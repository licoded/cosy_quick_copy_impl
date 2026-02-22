#pragma once

#include <cudd/cuddObj.hh>

namespace Cosy {

/**
 * @brief BDD 逻辑检查工具类
 */
class BddChecker
{
public:
    BddChecker() = delete;

    static bool checkImplies(const CUDD::BDD& f1, const CUDD::BDD& f2, const CUDD::BDD& falseBdd)
    {
        return checkConflicts(f1, !f2, falseBdd);
    }

    static bool checkConflicts(const CUDD::BDD& f1, const CUDD::BDD& f2, const CUDD::BDD& falseBdd)
    {
        return (f1 & f2) == falseBdd;
    }
};

}  // namespace Cosy
