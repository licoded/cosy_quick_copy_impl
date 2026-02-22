#include "cudd/formula_in_bdd.hpp"
#include "cudd/cudd_config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace Cosy {

void FormulaInBddMgr::buildClauses(Formula* af)
{
    if (af == nullptr)
        return;
    if (afP_to_bddP_.count(uint64_t(af)))
        return;

    Operator op = af->op();

    // For literals (atoms), they should be initialized in fixAtomOrder
    if (op == Operator::Literal)
    {
        if (!hasBuilt(af))
        {
            exit_with_error("All atoms should be init in the beginning! Found uninitialized atom: " + af->toString());
        }
        return;
    }

    switch (op)
    {
        case Operator::True:
        case Operator::False:
            break;
        case Operator::Next:
        case Operator::WNext:
            buildIfMissing(af);
            break;
        case Operator::Not:
            buildClauses(af->right());
            break;
        case Operator::Until:
        case Operator::Release:
            if (is_tail(af) || is_not_tail(af))
                break;
            exit_with_error("Please convert the formula to XNF first!");
            break;
        case Operator::And:
        case Operator::Or:
            buildClauses(af->left());
            buildClauses(af->right());
            break;
        default:
            break;
    }
}

CUDD::BDD FormulaInBddMgr::constructBdd(Formula* af)
{
    if (af == nullptr)
        exit_with_error("[constructBdd] the formula is NULL!");

    auto it = afP_to_bddP_.find(uint64_t(af));
    if (it != afP_to_bddP_.end())
    {
        return it->second;  // 返回拷贝，自动管理引用
    }

    Operator op = af->op();
    CUDD::BDD res;

    switch (op)
    {
        case Operator::Not:
        {
            CUDD::BDD tmp = constructBdd(af->right());
            res = !tmp;
            break;
        }
        case Operator::And:
        {
            CUDD::BDD l_bdd = constructBdd(af->left());
            CUDD::BDD r_bdd = constructBdd(af->right());
            res = l_bdd & r_bdd;
            break;
        }
        case Operator::Or:
        {
            CUDD::BDD l_bdd = constructBdd(af->left());
            CUDD::BDD r_bdd = constructBdd(af->right());
            res = l_bdd | r_bdd;
            break;
        }
        default:
        {
            spdlog::error("[constructBdd] for {}", af->toString());
            exit_with_error("[constructBdd] Atom, Next, WNext should be already built!");
        }
    }

    afP_to_bddP_.insert({uint64_t(af), res});
    return res;
}

CUDD::BDD FormulaInBddMgr::convertFormula2Bdd(Formula* af)
{
    if (afP_to_bddP_.find(uint64_t(af)) == afP_to_bddP_.end())
        constructBdd(af);
    return afP_to_bddP_.at(uint64_t(af));
}

bool FormulaInBddMgr::CheckImplies(const CUDD::BDD& f1, const CUDD::BDD& f2)
{
    // f1 => f2 等价于 f1 & !f2 == false
    CUDD::BDD not_f2 = !f2;
    return CheckConflicts(f1, not_f2);
}

bool FormulaInBddMgr::CheckImplies(Formula* edge_af1, Formula* edge_af2)
{
    CUDD::BDD f1_bdd = convertFormula2Bdd(edge_af1);
    CUDD::BDD f2_bdd = convertFormula2Bdd(edge_af2);
    return CheckImplies(f1_bdd, f2_bdd);
}

bool FormulaInBddMgr::CheckConflicts(const CUDD::BDD& f1, const CUDD::BDD& f2)
{
    // f1 和 f2 冲突等价于 f1 & f2 == false
    CUDD::BDD f1_and_f2 = f1 & f2;
    return f1_and_f2 == falseBdd();
}

bool FormulaInBddMgr::CheckConflicts(Formula* edge_af1, Formula* edge_af2)
{
    CUDD::BDD f1_bdd = convertFormula2Bdd(edge_af1);
    CUDD::BDD f2_bdd = convertFormula2Bdd(edge_af2);
    return CheckConflicts(f1_bdd, f2_bdd);
}

}  // namespace Cosy
