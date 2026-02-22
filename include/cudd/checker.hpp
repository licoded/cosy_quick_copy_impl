#pragma once

#include <cudd/cuddObj.hh>

namespace Cosy {

class CuddMgr;
class Formula;

/**
 * @brief BDD 逻辑检查工具类
 *
 * 无状态工具类，提供 BDD 蕴含和冲突检查。
 */
class BddChecker
{
public:
    BddChecker() = delete;  // 纯静态工具类

    // === BDD 版本 ===

    static bool checkImplies(const CUDD::BDD& f1, const CUDD::BDD& f2, const CUDD::BDD& falseBdd)
    {
        CUDD::BDD not_f2 = !f2;
        return checkConflicts(f1, not_f2, falseBdd);
    }

    static bool checkConflicts(const CUDD::BDD& f1, const CUDD::BDD& f2, const CUDD::BDD& falseBdd)
    {
        return (f1 & f2) == falseBdd;
    }

    // === Formula 版本 ===

    static bool checkImplies(CuddMgr& mgr, Formula* f1, Formula* f2);
    static bool checkConflicts(CuddMgr& mgr, Formula* f1, Formula* f2);
};

}  // namespace Cosy
