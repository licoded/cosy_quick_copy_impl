#pragma once

#include "cudd/cudd_config.hpp"
#include "cudd/cudd_mgr_base.hpp"
#include "formula/utils.hpp"
#include "cudd/part_var.hpp"
#include "formula/formula.hpp"
#include "formula/builder.hpp"
#include <algorithm>
#include <cudd/cuddObj.hh>
#include <iostream>
#include <map>
#include <set>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>

template <>
struct fmt::formatter<Cosy::Formula> : fmt::formatter<std::string>
{
    auto format(Cosy::Formula af, format_context& ctx) const -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}", af.toString());
    }
};

namespace Cosy {

class FormulaInBddMgr;

class FormulaInBdd : public IHashId
{
  private:
    Formula* formula_;
    CUDD::BDD bdd_;

  public:
    FormulaInBdd(Formula* af, CUDD::BDD bdd) : formula_(af), bdd_(std::move(bdd)) {}

    CUDD::BDD getBdd() const { return bdd_; }
    DdNode* getBddNode() const { return bdd_.getNode(); }
    Formula* getAfP() const { return formula_; }
    uint64_t getHashId() const override { return reinterpret_cast<uint64_t>(bdd_.getNode()); }
    std::string toString() const override { return fmt::format("[{}]: {}", getHashId(), getAfP()->toString()); }

    ~FormulaInBdd() override = default;
};

class FormulaInBddMgr : public ICuddMgr
{
  private:
    std::vector<std::string> af_str_vec_;
    std::unordered_map<uint64_t, CUDD::BDD> afP_to_bddP_;

    bool hasBuilt(Formula* af) { return afP_to_bddP_.find(uint64_t(af)) != afP_to_bddP_.end(); }

    void recordBuiltMap(Formula* af, CUDD::BDD bdd)
    {
        afP_vec_.push_back(af);
        afP_to_bddP_.insert({uint64_t(af), std::move(bdd)});
    }

    void buildIfMissing(Formula* af)
    {
        if (!hasBuilt(af))
            recordBuiltMap(af, newBddVar());
    }

    CUDD::BDD getBdd(Formula* af) { return afP_to_bddP_.at(uint64_t(af)); }

    void buildClauses(Formula* af);
    CUDD::BDD constructBdd(Formula* af);
    CUDD::BDD convertFormula2Bdd(Formula* af);

    std::vector<Formula*> getAtomsWithBuilder()
    {
        std::vector<Formula*> atoms;
        for (int varId : part_var_.getYVarIds())
        {
            atoms.push_back(builder_.make_literal(varId));
        }
        for (int varId : part_var_.getXVarIds())
        {
            atoms.push_back(builder_.make_literal(varId));
        }
        return atoms;
    }

    void fixAtomOrder() override
    {
        std::vector<Formula*> atoms = getAtomsWithBuilder();
        for (auto atom : atoms)
            buildIfMissing(atom);
        for (auto atom : atoms)
            var_names_.push_back(atom->toString());
    }

    void initTailBdd()
    {
        Formula* tail = builder_.make_tail();
        assert(!hasBuilt(tail));
        CUDD::BDD tail_bdd = newBddVar();
        recordBuiltMap(tail, tail_bdd);

        Formula* not_tail = builder_.make_not_tail();
        CUDD::BDD not_tail_bdd = !tail_bdd;
        recordBuiltMap(not_tail, not_tail_bdd);
    }

  public:
    explicit FormulaInBddMgr(PartVar part_var, FormulaBuilder& builder)
        : ICuddMgr(part_var, builder)
    {
        fixAtomOrder();
        initTailBdd();
        afP_to_bddP_.insert({uint64_t(builder_.make_true()), trueBdd()});
        afP_to_bddP_.insert({uint64_t(builder_.make_false()), falseBdd()});
    }

    FormulaInBdd* createFormulaInBdd(Formula* af, Formula* xnf_af)
    {
        buildClauses(xnf_af);
        CUDD::BDD bdd = convertFormula2Bdd(xnf_af);
        spdlog::debug("FormulaInBddMgr::createFormulaInBdd\n{}\n{}\n{}", af->toString(), xnf_af->toString(),
                      reinterpret_cast<uint64_t>(bdd.getNode()));
        return new FormulaInBdd(af, std::move(bdd));
    }

    Formula* getCurAfVar(DdNode* bddP) const override
    {
        CUDD::BDD bdd(const_cast<CUDD::Cudd&>(cudd_), bddP);
        if (!isXYVar(bdd, part_var_.getAllVarNum()))
            exit_with_error("[getCurAfVar] BDD node is not a variable!");
        return afP_vec_[bdd.NodeReadIndex()];
    }

    bool CheckImplies(const CUDD::BDD& f1, const CUDD::BDD& f2);
    bool CheckImplies(Formula* edge_af1, Formula* edge_af2);

    bool CheckConflicts(const CUDD::BDD& f1, const CUDD::BDD& f2);
    bool CheckConflicts(Formula* edge_af1, Formula* edge_af2);
};

}  // namespace Cosy
