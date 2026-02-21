#include "synutil/formula_in_bdd.h"
#include "debug.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <queue>
#include <vector>

#include "formula/aalta_formula.h"
#include <cudd/cuddObj.hh>

using namespace std;
using namespace aalta;

namespace syn_util {

void FormulaInBddMgr::buildClauses(aalta_formula *af)
{
    if (af == NULL)
        return;
    if (afP_to_bddP_.find(u_int64_t(af)) != afP_to_bddP_.end())
        return;
    int op = af->oper();
    if (op >= 11)
    {
        exit_with_error("All atoms should be init in the beginning! Found uninitialized atom: " + af->to_string());
        return;
    }
    switch (op)
    {
    case aalta_formula::True:
    case aalta_formula::False:
        break;
    case aalta_formula::Next:
    case aalta_formula::WNext:
        buildIfMissing(af);
        break;
    case aalta_formula::Not:
        buildClauses(af->r_af());
        break;
    case aalta_formula::Until:
    case aalta_formula::Release:
        if (af == aalta_formula::TAIL() || af == aalta_formula::NOT_TAIL())
            break;
        exit_with_error("Please convert the formula to XNF first!");
        // aalta_formula::opkind next_op = (op == aalta_formula::Until) ? aalta_formula::Next : aalta_formula::WNext;
        // aalta_formula *next_af = aalta_formula(op, NULL, af).unique();
        // buildIfMissing(next_af);
        break;
    case aalta_formula::And:
    case aalta_formula::Or:
        buildClauses(af->l_af());
        buildClauses(af->r_af());
        break;
    }
}

DdNode *FormulaInBddMgr::constructBdd(aalta_formula *af)
{
    if (af == NULL)
        exit_with_error("[constructBdd] the formula is NULL!");
    if (afP_to_bddP_.find(u_int64_t(af)) != afP_to_bddP_.end())
    {
        DdNode *cache_node = afP_to_bddP_.at(u_int64_t(af));
        return Cudd_Ref_Wrapper(cache_node);
    }
    int op = af->oper();
    DdNode *res_node = nullptr;
    switch (op)
    {
    case aalta_formula::Not:
    {
        DdNode *tmp = constructBdd(af->r_af());
        DdNode *not_tmp = Cudd_Not(tmp);
        res_node = Cudd_Ref_Wrapper(not_tmp);
        Cudd_Unref(tmp);
        break;
    }
    case aalta_formula::And:
    case aalta_formula::Or:
    {
        DdNode *l_bdd = constructBdd(af->l_af());
        DdNode *r_bdd = constructBdd(af->r_af());
        DdNode *result = (op == aalta_formula::And) ? Cudd_bddAnd(l_bdd, r_bdd) : Cudd_bddOr(l_bdd, r_bdd);
        res_node = Cudd_Ref_Wrapper(result);
        Cudd_Unref(l_bdd);
        Cudd_Unref(r_bdd);
        break;
    }
    default: // Atom, Next, WNext
    {
        spdlog::error("[constructBdd] for {}", af->to_string());
        exit_with_error("[constructBdd] Atom, Next, WNext should be already built!");
    }
    }
    afP_to_bddP_.insert({u_int64_t(af), res_node});
    return Cudd_Ref_Wrapper(res_node);
}

DdNode *FormulaInBddMgr::convertFormula2Bdd(aalta_formula *af)
{
    if (afP_to_bddP_.find(u_int64_t(af)) == afP_to_bddP_.end())
        constructBdd(af);
    return afP_to_bddP_.at(u_int64_t(af));
}

bool FormulaInBddMgr::CheckImplies(DdNode *f1, DdNode *f2)
{
    DdNode *not_f2 = Cudd_bddNot(f2);
    Cudd_Ref(not_f2);
    DdNode *f1_and_not_f2 = Cudd_bddAnd(f1, not_f2);
    Cudd_Ref(f1_and_not_f2);
    Cudd_Unref(not_f2);
    bool res_flag = f1_and_not_f2 == FALSE_bddP_;
    Cudd_Unref(f1_and_not_f2);
    return res_flag;
}

bool FormulaInBddMgr::CheckImplies(aalta::aalta_formula *edge_af1, aalta::aalta_formula *edge_af2)
{
    DdNode *f1_bdd = convertFormula2Bdd(edge_af1);
    DdNode *f2_bdd = convertFormula2Bdd(edge_af2);
    bool is_conflict = CheckImplies(f1_bdd, f2_bdd);
    return is_conflict;
}

bool FormulaInBddMgr::CheckConflicts(DdNode *f1, DdNode *f2)
{
    DdNode *f1_and_f2 = Cudd_bddAnd(f1, f2);
    Cudd_Ref(f1_and_f2);
    bool res_flag = f1_and_f2 == FALSE_bddP_;
    Cudd_Unref(f1_and_f2);
    return res_flag;
}

bool FormulaInBddMgr::CheckConflicts(aalta::aalta_formula *edge_af1, aalta::aalta_formula *edge_af2)
{
    DdNode *f1_bdd = convertFormula2Bdd(edge_af1);
    DdNode *f2_bdd = convertFormula2Bdd(edge_af2);
    bool is_conflict = CheckConflicts(f1_bdd, f2_bdd);
    return is_conflict;
}

} // namespace syn_util
