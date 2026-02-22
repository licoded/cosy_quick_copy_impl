#pragma once
#include "cudd/cudd_config.hpp"
#include "cudd/part_var.hpp"
#include "formula/formula.hpp"
#include "formula/operator.hpp"
#include <algorithm>
#include <cudd/cuddObj.hh>
#include <spdlog/spdlog.h>
#include <unordered_set>
#include <vector>

namespace Cosy {

class ICuddMgr
{
    // === for part_var ===
  protected:
    PartVar part_var_;
    FormulaBuilder& builder_;

  public:
    PartVar& getPartVar() { return part_var_; }
    FormulaBuilder& getBuilder() { return builder_; }
    auto getAllVarIds() const { return part_var_.getAllVarIds(); }
    std::pair<Formula*, Formula*>* split_XY_from_edgeAf(Formula* af);

    // === for cudd_mgr ===
  protected:
    CUDD::Cudd cudd_;
    CUDD::BDD true_bdd_;
    CUDD::BDD false_bdd_;
    std::vector<std::string> var_names_;

  public:
    CUDD::Cudd& cudd() { return cudd_; }
    DdManager* getManager() { return cudd_.getManager(); }

    CUDD::BDD trueBdd() const { return true_bdd_; }
    CUDD::BDD falseBdd() const { return false_bdd_; }

    CUDD::BDD newBddVar() { return cudd_.bddVar(); }

    // 辅助函数：从 BDD 获取裸指针
    static DdNode* getNode(const CUDD::BDD& bdd) { return bdd.getNode(); }

    // 变量索引相关
    static bool isYVar(const CUDD::BDD& bdd, int y_var_num) {
        return bdd.NodeReadIndex() < y_var_num;
    }
    static bool isXYVar(const CUDD::BDD& bdd, int all_var_num) {
        return bdd.NodeReadIndex() < all_var_num;
    }

    // === fixAtomOrder ===
  protected:
    virtual void fixAtomOrder() = 0;

    // === ctor and dtor
  public:
    explicit ICuddMgr(PartVar part_var, FormulaBuilder& builder);
    virtual ~ICuddMgr();

    // === trans in cudd tree
  protected:
    std::vector<Formula*> afP_vec_;  // 存储 Formula 指针，用于获取 var_id

    // === for edge_cons_builder (保留裸指针接口，某些操作需要)
  public:
    virtual Formula* getCurAfVar(DdNode* cuddP) const = 0;
};

}  // namespace Cosy
