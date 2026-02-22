#pragma once

#include <cudd/cuddObj.hh>

namespace Cosy {

/**
 * @brief BDD 逻辑检查工具类
 *
 * 无状态工具类，提供 BDD 蕴含和冲突检查。
 */
class BddChecker
{
public:
    BddChecker() = delete;  // 纯静态工具类

    /**
     * @brief 检查 f1 是否蕴含 f2
     * @param f1 前提
     * @param f2 结论
     * @param falseBdd 用于判断是否为 false 的常量
     * @return true 如果 f1 => f2
     */
    static bool checkImplies(const CUDD::BDD& f1, const CUDD::BDD& f2, const CUDD::BDD& falseBdd)
    {
        // f1 => f2 等价于 f1 & !f2 == false
        CUDD::BDD not_f2 = !f2;
        return checkConflicts(f1, not_f2, falseBdd);
    }

    /**
     * @brief 检查 f1 和 f2 是否冲突（不可同时为真）
     * @param f1 第一个 BDD
     * @param f2 第二个 BDD
     * @param falseBdd 用于判断是否为 false 的常量
     * @return true 如果 f1 & f2 == false
     */
    static bool checkConflicts(const CUDD::BDD& f1, const CUDD::BDD& f2, const CUDD::BDD& falseBdd)
    {
        CUDD::BDD f1_and_f2 = f1 & f2;
        return f1_and_f2 == falseBdd;
    }
};

}  // namespace Cosy
