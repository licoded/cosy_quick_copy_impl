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
    registerAtomsInOrder();
    initTailBdd();
    initTrueFalse();
}

void CuddMgr::registerClausesAsBddVars(Formula* af)
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
            registerClausesAsBddVars(af->right());
            break;
        case Operator::Until:
        case Operator::Release:
            if (is_tail(af) || is_not_tail(af))
                break;
            exit_with_error("Please convert the formula to XNF first!");
            break;
        case Operator::And:
        case Operator::Or:
            registerClausesAsBddVars(af->left());
            registerClausesAsBddVars(af->right());
            break;
        default:
            break;
    }
}

CUDD::BDD CuddMgr::formulaToBdd(Formula* af)
{
    if (af == nullptr)
        exit_with_error("[formulaToBdd] the formula is NULL!");

    auto it = cache_.getAfPToBddP().find(reinterpret_cast<uint64_t>(af));
    if (it != cache_.getAfPToBddP().end())
        return it->second;

    Operator op = af->op();
    CUDD::BDD res;

    switch (op)
    {
        case Operator::Not:
            res = !formulaToBdd(af->right());
            break;
        case Operator::And:
            res = formulaToBdd(af->left()) & formulaToBdd(af->right());
            break;
        case Operator::Or:
            res = formulaToBdd(af->left()) | formulaToBdd(af->right());
            break;
        default:
            spdlog::error("[formulaToBdd] for {}", af->toString());
            exit_with_error("[formulaToBdd] Atom, Next, WNext should be already built!");
    }

    cache_.recordWithoutVec(af, res);
    return res;
}

void CuddMgr::registerAtomsInOrder()
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
    registerClausesAsBddVars(xnf_af);
    CUDD::BDD bdd = formulaToBdd(xnf_af);
    spdlog::debug("CuddMgr::createFormulaInBdd\n{}\n{}\n{}",
                  af->toString(), xnf_af->toString(),
                  reinterpret_cast<uint64_t>(bdd.getNode()));
    return new FormulaInBdd(af, std::move(bdd));
}

Formula* CuddMgr::atomFormulaFromBdd(DdNode* bddP) const
{
    CUDD::BDD bdd(const_cast<CUDD::Cudd&>(core_.cudd()), bddP);
    if (!isAtomVar(bdd, static_cast<int>(var_mgr_.getAllVarNum())))
        exit_with_error("[atomFormulaFromBdd] BDD node is not an atom variable!");
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
    return checkImplies(formulaToBdd(f1), formulaToBdd(f2));
}

bool CuddMgr::checkConflicts(Formula* f1, Formula* f2)
{
    return checkConflicts(formulaToBdd(f1), formulaToBdd(f2));
}

}  // namespace Cosy
