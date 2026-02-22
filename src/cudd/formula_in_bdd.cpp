#include "cudd/formula_in_bdd.hpp"
#include "cudd/cudd_config.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <queue>
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
    if (op == Operator::Literal) {
        // Check if this is a known literal
        if (!hasBuilt(af)) {
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
            // Check for tail/not_tail
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

DdNode *FormulaInBddMgr::constructBdd(Formula* af)
{
    if (af == nullptr)
        exit_with_error("[constructBdd] the formula is NULL!");
    if (afP_to_bddP_.find(uint64_t(af)) != afP_to_bddP_.end())
    {
        DdNode *cache_node = afP_to_bddP_.at(uint64_t(af));
        return Cudd_Ref_Wrapper(cache_node);
    }

    Operator op = af->op();
    DdNode *res_node = nullptr;

    switch (op)
    {
        case Operator::Not:
        {
            DdNode *tmp = constructBdd(af->right());
            DdNode *not_tmp = Cudd_Not(tmp);
            res_node = Cudd_Ref_Wrapper(not_tmp);
            Cudd_Unref(tmp);
            break;
        }
        case Operator::And:
        case Operator::Or:
        {
            DdNode *l_bdd = constructBdd(af->left());
            DdNode *r_bdd = constructBdd(af->right());
            DdNode *result = (op == Operator::And) ? Cudd_bddAnd(l_bdd, r_bdd) : Cudd_bddOr(l_bdd, r_bdd);
            res_node = Cudd_Ref_Wrapper(result);
            Cudd_Unref(l_bdd);
            Cudd_Unref(r_bdd);
            break;
        }
        default: // Atom, Next, WNext
        {
            spdlog::error("[constructBdd] for {}", af->toString());
            exit_with_error("[constructBdd] Atom, Next, WNext should be already built!");
        }
    }

    afP_to_bddP_.insert({uint64_t(af), res_node});
    return Cudd_Ref_Wrapper(res_node);
}

DdNode *FormulaInBddMgr::convertFormula2Bdd(Formula* af)
{
    if (afP_to_bddP_.find(uint64_t(af)) == afP_to_bddP_.end())
        constructBdd(af);
    return afP_to_bddP_.at(uint64_t(af));
}

bool FormulaInBddMgr::CheckImplies(DdNode *f1, DdNode *f2)
{
    DdNode *not_f2 = Cudd_bddNot(f2);
    Cudd_Ref(not_f2);
    bool res_flag = CheckConflicts(f1, not_f2);
    Cudd_Unref(not_f2);
    return res_flag;
}

bool FormulaInBddMgr::CheckImplies(Formula* edge_af1, Formula* edge_af2)
{
    DdNode *f1_bdd = convertFormula2Bdd(edge_af1);
    DdNode *f2_bdd = convertFormula2Bdd(edge_af2);
    bool res_flag = CheckImplies(f1_bdd, f2_bdd);
    return res_flag;
}

bool FormulaInBddMgr::CheckConflicts(DdNode *f1, DdNode *f2)
{
    DdNode *f1_and_f2 = Cudd_bddAnd(f1, f2);
    Cudd_Ref(f1_and_f2);
    bool res_flag = f1_and_f2 == FALSE_bddP_;
    Cudd_Unref(f1_and_f2);
    return res_flag;
}

bool FormulaInBddMgr::CheckConflicts(Formula* edge_af1, Formula* edge_af2)
{
    DdNode *f1_bdd = convertFormula2Bdd(edge_af1);
    DdNode *f2_bdd = convertFormula2Bdd(edge_af2);
    bool is_conflict = CheckConflicts(f1_bdd, f2_bdd);
    return is_conflict;
}

} // namespace Cosy
