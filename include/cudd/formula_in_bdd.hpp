#pragma once

#include "debug.h"
#include "formula/aalta_formula.h"
#include "formula/af_utils.h"
#include "synutil/cudd_mgr_base.h"
#include "synutil/hash_id.h"
#include "synutil/part_var.h"
#include <algorithm>
#include <cudd/cuddObj.hh>
#include <iostream>
#include <map>
#include <set>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>

template <> struct fmt::formatter<aalta::aalta_formula> : fmt::formatter<std::string> {
    auto format(aalta::aalta_formula af, format_context &ctx) const -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}", af.to_string());
    }
};

namespace syn_util {

class CuddMgr;
class FormulaInBddMgr;
class FormulaInBdd;

class FormulaInBdd : public IHashId
{
  private:
    aalta::aalta_formula *formula_;
    DdNode *bdd_;

  public:
    FormulaInBdd(aalta::aalta_formula *af, DdNode *bdd) : formula_(af), bdd_(bdd) {}

    inline DdNode *getBddP() const { return bdd_; }
    inline aalta::aalta_formula *getAfP() const { return formula_; }
    inline u_int64_t getHashId() const override { return u_int64_t(getBddP()); }
    std::string toString() const override { return fmt::format("[{}]: {}", getHashId(), getAfP()->to_string()); }

    ~FormulaInBdd() override {}
};

class FormulaInBddMgr : public ICuddMgr
{
  private:
    std::vector<aalta::aalta_formula *> afP_vec_;
    std::vector<std::string> af_str_vec_;
    std::vector<DdNode *> bddP_vec_;
    std::unordered_map<u_int64_t, DdNode *> afP_to_bddP_;
    bool hasBuilt(aalta::aalta_formula *af) { return afP_to_bddP_.find(u_int64_t(af)) != afP_to_bddP_.end(); }
    void recordBuiltMap(aalta::aalta_formula *af, DdNode *bdd)
    {
        afP_vec_.push_back(af);
        // af_str_vec_.push_back(af->to_string());
        afP_to_bddP_.insert({u_int64_t(af), bdd});
        // bddP_vec_.push_back(bdd);
        af_atomOper_vec_.push_back(af->oper());
    }
    void buildIfMissing(aalta::aalta_formula *af)
    {
        if (!hasBuilt(af))
            recordBuiltMap(af, newBddVar());
    }
    DdNode *getBdd(aalta::aalta_formula *af) { return afP_to_bddP_.at(u_int64_t(af)); }
    void buildClauses(aalta::aalta_formula *af);
    DdNode *constructBdd(aalta::aalta_formula *af);
    DdNode *convertFormula2Bdd(aalta::aalta_formula *af);
    void fixAtomOrder(const std::vector<aalta::aalta_formula *> &atoms) override
    {
        for (auto atom : atoms)
            buildIfMissing(atom);
        for (auto atom : atoms)
            var_names_.push_back(atom->to_string());
    }
    void initTailBdd()
    {
        aalta::aalta_formula *tail = aalta::aalta_formula::TAIL();
        assert(!hasBuilt(tail));
        DdNode *tail_bdd = newBddVar();
        recordBuiltMap(tail, tail_bdd);
        aalta::aalta_formula *not_tail = aalta::aalta_formula::NOT_TAIL();
        DdNode *not_tail_bdd = Cudd_Not(tail_bdd);
        Cudd_Ref(not_tail_bdd);
        recordBuiltMap(not_tail, not_tail_bdd);
    }

  public:
    explicit FormulaInBddMgr(PartVar part_var) : ICuddMgr(part_var)
    {
        fixAtomOrder(getAtoms());
        initTailBdd();
        afP_to_bddP_.insert({u_int64_t(aalta::aalta_formula::TRUE()), TRUE_bddP_});
        afP_to_bddP_.insert({u_int64_t(aalta::aalta_formula::FALSE()), FALSE_bddP_});
    }

    FormulaInBdd *createFormulaInBdd(aalta::aalta_formula *af, aalta::aalta_formula *xnf_af)
    {
        buildClauses(xnf_af);
        DdNode *bdd = convertFormula2Bdd(xnf_af);
        spdlog::debug("FormulaInBddMgr::createFormulaInBdd\n{}\n{}\n{}", *af, *xnf_af, u_int64_t(bdd));
        return new FormulaInBdd(af, bdd);
    }

    aalta::aalta_formula *getCurAfVar(DdNode *bddP) const override
    {
        if (!isXYVar(bddP))
            exit_with_error("[getCurAfVar] BDD node is not a variable!");
        return afP_vec_[Cudd_NodeReadIndex(bddP)];
    }

    DdNode *getRealCuddP(DdNode *bddP) override { return Cudd_IsComplement(bddP) ? Cudd_Regular(bddP) : bddP; }

    bool CheckImplies(DdNode *f1, DdNode *f2);
    bool CheckImplies(aalta::aalta_formula *edge_af1, aalta::aalta_formula *edge_af2);

    bool CheckConflicts(DdNode *f1, DdNode *f2);
    bool CheckConflicts(aalta::aalta_formula *edge_af1, aalta::aalta_formula *edge_af2);
};

} // namespace syn_util
