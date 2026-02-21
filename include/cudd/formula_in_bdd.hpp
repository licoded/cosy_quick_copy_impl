#pragma once

#include "cudd/cudd_config.hpp"
#include "cudd/cudd_mgr_base.hpp"
#include "cudd/formula_utils.hpp"
#include "cudd/part_var.hpp"
#include "formula/formula.hpp"
#include "formula/builder.hpp"
#include "formula/variable_collector.hpp"
#include <algorithm>
#include <cudd/cuddObj.hh>
#include <iostream>
#include <map>
#include <set>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>

template <> struct fmt::formatter<Cosy::Formula> : fmt::formatter<std::string> {
    auto format(Cosy::Formula af, format_context &ctx) const -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}", af.toString());
    }
};

namespace Cosy {

class CuddMgr;
class FormulaInBddMgr;
class FormulaInBdd;

class FormulaInBdd : public IHashId
{
  private:
    Formula* formula_;
    DdNode *bdd_;

  public:
    FormulaInBdd(Formula* af, DdNode *bdd) : formula_(af), bdd_(bdd) {}

    inline DdNode *getBddP() const { return bdd_; }
    inline Formula* getAfP() const { return formula_; }
    inline uint64_t getHashId() const override { return uint64_t(getBddP()); }
    std::string toString() const override { return fmt::format("[{}]: {}", getHashId(), getAfP()->toString()); }

    ~FormulaInBdd() override {}
};

class FormulaInBddMgr : public ICuddMgr
{
  private:
    std::vector<Formula*> afP_vec_;
    std::vector<std::string> af_str_vec_;
    std::vector<DdNode *> bddP_vec_;
    std::unordered_map<uint64_t, DdNode *> afP_to_bddP_;
    bool hasBuilt(Formula* af) { return afP_to_bddP_.find(uint64_t(af)) != afP_to_bddP_.end(); }
    void recordBuiltMap(Formula* af, DdNode *bdd)
    {
        afP_vec_.push_back(af);
        afP_to_bddP_.insert({uint64_t(af), bdd});
        // Store the operator type for atom operations
        af_atomOper_vec_.push_back(static_cast<unsigned int>(af->op()));
    }
    void buildIfMissing(Formula* af)
    {
        if (!hasBuilt(af))
            recordBuiltMap(af, newBddVar());
    }
    DdNode *getBdd(Formula* af) { return afP_to_bddP_.at(uint64_t(af)); }
    void buildClauses(Formula* af);
    DdNode *constructBdd(Formula* af);
    DdNode *convertFormula2Bdd(Formula* af);

    std::vector<Formula*> getAtomsWithBuilder()
    {
        std::vector<Formula*> atoms;
        for (int varId : part_var_.getYVarIds()) {
            atoms.push_back(builder_.make_literal(get_global_symbol_table().get_var_name(varId)));
        }
        for (int varId : part_var_.getXVarIds()) {
            atoms.push_back(builder_.make_literal(get_global_symbol_table().get_var_name(varId)));
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
        DdNode *tail_bdd = newBddVar();
        recordBuiltMap(tail, tail_bdd);
        Formula* not_tail = builder_.make_not_tail();
        DdNode *not_tail_bdd = Cudd_Not(tail_bdd);
        Cudd_Ref(not_tail_bdd);
        recordBuiltMap(not_tail, not_tail_bdd);
    }

  public:
    explicit FormulaInBddMgr(PartVar part_var, FormulaBuilder& builder)
        : ICuddMgr(part_var, builder)
    {
        fixAtomOrder();
        initTailBdd();
        Formula* true_f = builder_.make_true();
        Formula* false_f = builder_.make_false();
        afP_to_bddP_.insert({uint64_t(true_f), TRUE_bddP_});
        afP_to_bddP_.insert({uint64_t(false_f), FALSE_bddP_});
    }

    FormulaInBdd *createFormulaInBdd(Formula* af, Formula* xnf_af)
    {
        buildClauses(xnf_af);
        DdNode *bdd = convertFormula2Bdd(xnf_af);
        spdlog::debug("FormulaInBddMgr::createFormulaInBdd\n{}\n{}\n{}", af->toString(), xnf_af->toString(), uint64_t(bdd));
        return new FormulaInBdd(af, bdd);
    }

    Formula* getCurAfVar(DdNode *bddP) const override
    {
        if (!isXYVar(bddP))
            exit_with_error("[getCurAfVar] BDD node is not a variable!");
        return afP_vec_[Cudd_NodeReadIndex(bddP)];
    }

    DdNode *getRealCuddP(DdNode *bddP) override { return Cudd_IsComplement(bddP) ? Cudd_Regular(bddP) : bddP; }

    bool CheckImplies(DdNode *f1, DdNode *f2);
    bool CheckImplies(Formula* edge_af1, Formula* edge_af2);

    bool CheckConflicts(DdNode *f1, DdNode *f2);
    bool CheckConflicts(Formula* edge_af1, Formula* edge_af2);
};

} // namespace Cosy
