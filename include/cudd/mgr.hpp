#pragma once

#include "cudd/checker.hpp"
#include "cudd/core.hpp"
#include "cudd/formula_cache.hpp"
#include "cudd/formula_in_bdd.hpp"
#include "cudd/var_manager.hpp"
#include "formula/formula.hpp"
#include <cudd/cuddObj.hh>
#include <vector>

namespace Cosy {

/**
 * @brief Cudd 管理器门面
 *
 * 组合所有 CUDD 相关组件，对外提供统一接口。
 */
class CuddMgr
{
private:
    CuddCore core_;
    BddVarManager var_mgr_;
    FormulaBddCache cache_;
    std::vector<std::string> var_names_;

    void buildClauses(Formula* af);
    CUDD::BDD constructBdd(Formula* af);
    void fixAtomOrder();
    void initTailBdd();

    void initTrueFalse()
    {
        cache_.recordWithoutVec(var_mgr_.makeTrue(), core_.trueBdd());
        cache_.recordWithoutVec(var_mgr_.makeFalse(), core_.falseBdd());
    }

    CUDD::BDD convertFormula2Bdd(Formula* af);

public:
    explicit CuddMgr(PartVar part_var, FormulaBuilder& builder);

    CuddMgr(const CuddMgr&) = delete;
    CuddMgr& operator=(const CuddMgr&) = delete;

    // === CuddCore 委托 ===
    CUDD::Cudd& cudd() { return core_.cudd(); }
    CUDD::BDD trueBdd() const { return core_.trueBdd(); }
    CUDD::BDD falseBdd() const { return core_.falseBdd(); }
    CUDD::BDD newBddVar() { return core_.newBddVar(); }

    // === BddVarManager 委托 ===
    PartVar& getPartVar() { return var_mgr_.getPartVar(); }
    FormulaBuilder& getBuilder() { return var_mgr_.getBuilder(); }
    auto getAllVarIds() const { return var_mgr_.getAllVarIds(); }
    std::pair<Formula*, Formula*>* splitXY(Formula* af) { return var_mgr_.splitXY(af); }

    // === 静态工具方法 ===
    static bool isYVar(const CUDD::BDD& bdd, int y_var_num) { return CuddCore::isYVar(bdd, y_var_num); }
    static bool isXYVar(const CUDD::BDD& bdd, int all_var_num) { return CuddCore::isXYVar(bdd, all_var_num); }

    // === Formula → BDD ===
    FormulaInBdd* createFormulaInBdd(Formula* af, Formula* xnf_af);
    FormulaInBdd* createFormulaInBdd(Formula* af) { return createFormulaInBdd(af, af->nnf()->xnf()); }
    Formula* getCurAfVar(DdNode* bddP) const;

    // === 逻辑检查 ===
    bool checkImplies(const CUDD::BDD& f1, const CUDD::BDD& f2) const;
    bool checkConflicts(const CUDD::BDD& f1, const CUDD::BDD& f2) const;
    bool checkImplies(Formula* f1, Formula* f2);
    bool checkConflicts(Formula* f1, Formula* f2);
};

}  // namespace Cosy
