#pragma once

#include "cudd/cudd_config.hpp"
#include <cudd/cuddObj.hh>

namespace Cosy {

/**
 * @brief CUDD 核心封装
 *
 * 纯 CUDD 管理功能，无业务逻辑依赖。
 * 负责 CUDD 环境的初始化、变量创建、常量 BDD 提供等。
 */
class CuddCore
{
protected:
    CUDD::Cudd cudd_;
    CUDD::BDD true_bdd_;
    CUDD::BDD false_bdd_;

public:
    CuddCore()
        : cudd_(0, 0, CUDD_UNIQUE_SLOTS * CUDD_TIMES_UNIQUE_SLOTS,
                CUDD_CACHE_SLOTS * CUDD_TIMES_CACHE_SLOTS)
        , true_bdd_(cudd_.bddOne())
        , false_bdd_(cudd_.bddZero())
    {
    }

    ~CuddCore() = default;

    // 禁止拷贝，允许移动
    CuddCore(const CuddCore&) = delete;
    CuddCore& operator=(const CuddCore&) = delete;
    CuddCore(CuddCore&&) = default;
    CuddCore& operator=(CuddCore&&) = default;

    // === CUDD 访问 ===
    CUDD::Cudd& cudd() { return cudd_; }
    const CUDD::Cudd& cudd() const { return cudd_; }
    DdManager* getManager() { return cudd_.getManager(); }
    const DdManager* getManager() const { return cudd_.getManager(); }

    // === 常量 BDD ===
    CUDD::BDD trueBdd() const { return true_bdd_; }
    CUDD::BDD falseBdd() const { return false_bdd_; }

    // === 变量创建 ===
    CUDD::BDD newBddVar() { return cudd_.bddVar(); }
    CUDD::BDD bddVar(int index) { return cudd_.bddVar(index); }

    // === 静态工具 ===
    static DdNode* getNode(const CUDD::BDD& bdd) { return bdd.getNode(); }

    static bool isYVar(const CUDD::BDD& bdd, int y_var_num)
    {
        return bdd.NodeReadIndex() < static_cast<unsigned int>(y_var_num);
    }

    static bool isXYVar(const CUDD::BDD& bdd, int all_var_num)
    {
        return bdd.NodeReadIndex() < static_cast<unsigned int>(all_var_num);
    }
};

}  // namespace Cosy
