#include "cudd/mgr.hpp"
#include "formula/operator.hpp"
#include "formula/utils.hpp"
#include <spdlog/spdlog.h>

namespace Cosy {

CuddMgr::CuddMgr(PartVar part_var, FormulaBuilder& builder)
    : core_()
    , var_mgr_(std::move(part_var), builder)
    , cache_()
    , var_names_()
{
    fixAtomOrder();
    initTailBdd();
    initTrueFalse();
}

void CuddMgr::buildClauses(Formula* af)
{
    if (af == nullptr || cache_.hasBuilt(af))
        return;

    Operator op = af->op();

    if (op == Operator::Literal)
    {
        if (!cache_.hasBuilt(af))
            exit_with_error("All atoms should be init in the beginning! Found uninitialized atom: " + af->toString());
        return;
    }

    switch (op)
    {
        case Operator::True:
        case Operator::False:
            break;
        case Operator::Next:
        case Operator::WNext:
            cache_.buildIfMissing(af, core_);
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

CUDD::BDD CuddMgr::constructBdd(Formula* af)
{
    if (af == nullptr)
        exit_with_error("[constructBdd] the formula is NULL!");

    auto it = cache_.getAfPToBddP().find(reinterpret_cast<uint64_t>(af));
    if (it != cache_.getAfPToBddP().end())
        return it->second;

    Operator op = af->op();
    CUDD::BDD res;

    switch (op)
    {
        case Operator::Not:
            res = !constructBdd(af->right());
            break;
        case Operator::And:
            res = constructBdd(af->left()) & constructBdd(af->right());
            break;
        case Operator::Or:
            res = constructBdd(af->left()) | constructBdd(af->right());
            break;
        default:
            spdlog::error("[constructBdd] for {}", af->toString());
            exit_with_error("[constructBdd] Atom, Next, WNext should be already built!");
    }

    cache_.recordWithoutVec(af, res);
    return res;
}

void CuddMgr::fixAtomOrder()
{
    std::vector<Formula*> atoms = var_mgr_.getAtoms();
    for (auto atom : atoms)
        cache_.buildIfMissing(atom, core_);
    for (auto atom : atoms)
        var_names_.push_back(atom->toString());
}

void CuddMgr::initTailBdd()
{
    Formula* tail = var_mgr_.makeTail();
    if (!cache_.hasBuilt(tail))
    {
        CUDD::BDD tail_bdd = core_.newBddVar();
        cache_.record(tail, tail_bdd);
        cache_.record(var_mgr_.makeNotTail(), !tail_bdd);
    }
}

FormulaInBdd* CuddMgr::createFormulaInBdd(Formula* af, Formula* xnf_af)
{
    buildClauses(xnf_af);
    CUDD::BDD bdd = convertFormula2Bdd(xnf_af);
    spdlog::debug("CuddMgr::createFormulaInBdd\n{}\n{}\n{}",
                  af->toString(), xnf_af->toString(),
                  reinterpret_cast<uint64_t>(bdd.getNode()));
    return new FormulaInBdd(af, std::move(bdd));
}

CUDD::BDD CuddMgr::convertFormula2Bdd(Formula* af)
{
    if (!cache_.hasBuilt(af))
        constructBdd(af);
    return cache_.getBdd(af);
}

Formula* CuddMgr::getCurAfVar(DdNode* bddP) const
{
    CUDD::BDD bdd(const_cast<CUDD::Cudd&>(core_.cudd()), bddP);
    if (!isXYVar(bdd, static_cast<int>(var_mgr_.getAllVarNum())))
        exit_with_error("[getCurAfVar] BDD node is not a variable!");
    return cache_.getFormulaByIndex(bdd.NodeReadIndex());
}

bool CuddMgr::checkImplies(const CUDD::BDD& f1, const CUDD::BDD& f2) const
{
    return BddChecker::checkImplies(f1, f2, core_.falseBdd());
}

bool CuddMgr::checkConflicts(const CUDD::BDD& f1, const CUDD::BDD& f2) const
{
    return BddChecker::checkConflicts(f1, f2, core_.falseBdd());
}

bool CuddMgr::checkImplies(Formula* f1, Formula* f2)
{
    return checkImplies(convertFormula2Bdd(f1), convertFormula2Bdd(f2));
}

bool CuddMgr::checkConflicts(Formula* f1, Formula* f2)
{
    return checkConflicts(convertFormula2Bdd(f1), convertFormula2Bdd(f2));
}

}  // namespace Cosy
