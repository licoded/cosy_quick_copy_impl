#pragma once

#include "cudd/core.hpp"
#include "formula/formula.hpp"
#include <cudd/cuddObj.hh>
#include <unordered_map>
#include <vector>

namespace Cosy {

/**
 * @brief Formula-BDD 映射缓存
 *
 * ltlf_props_: 只记录 atoms、tail/!tail, Next/WNext，这些才会被注册为 BDD 变量
 * formula_to_bdd_: Formula* → BDD
 */
class FormulaBddCache
{
protected:
    std::vector<Formula*> ltlf_props_;
    std::unordered_map<uint64_t, CUDD::BDD> formula_to_bdd_; // Formula* → BDD

public:
    FormulaBddCache() = default;
    ~FormulaBddCache() = default;

    // 禁止拷贝
    FormulaBddCache(const FormulaBddCache&) = delete;
    FormulaBddCache& operator=(const FormulaBddCache&) = delete;

    // === 查询 ===
    bool hasBuilt(Formula* af) const
    {
        return formula_to_bdd_.find(reinterpret_cast<uint64_t>(af)) != formula_to_bdd_.end();
    }

    CUDD::BDD getBdd(Formula* af) const
    {
        return formula_to_bdd_.at(reinterpret_cast<uint64_t>(af));
    }

    // === 记录 ===
    void mapProp2Bdd(Formula* af, CUDD::BDD bdd)
    {
        ltlf_props_.push_back(af);
        mapFormula2Bdd(af, std::move(bdd));
    }

    void mapFormula2Bdd(Formula* af, CUDD::BDD bdd)
    {
        formula_to_bdd_.insert({reinterpret_cast<uint64_t>(af), std::move(bdd)});
    }

    // === 构建（需要 CuddCore 依赖） ===
    void createBddVar4Prop(Formula* prop_formula, CuddCore& core)
    {
        mapProp2Bdd(prop_formula, core.newBddVar());
    }

    // === 访问内部数据（用于遍历等） ===
    const std::unordered_map<uint64_t, CUDD::BDD>& formulaToBddMap() const { return formula_to_bdd_; }
};

}  // namespace Cosy
