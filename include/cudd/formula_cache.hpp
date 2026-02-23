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
 * 纯数据存储，维护 Formula* ↔ BDD 的双向映射。
 * indexed_formulas_: BDD 索引 → Formula*
 * formula_to_bdd_: Formula* → BDD
 */
class FormulaBddCache
{
protected:
    std::vector<Formula*> indexed_formulas_;             // 索引 → Formula*
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

    Formula* getFormulaByIndex(size_t index) const
    {
        return indexed_formulas_.at(index);
    }

    size_t size() const { return indexed_formulas_.size(); }

    // === 记录 ===
    void record(Formula* af, CUDD::BDD bdd)
    {
        indexed_formulas_.push_back(af);
        formula_to_bdd_.insert({reinterpret_cast<uint64_t>(af), std::move(bdd)});
    }

    void recordWithoutVec(Formula* af, CUDD::BDD bdd)
    {
        formula_to_bdd_.insert({reinterpret_cast<uint64_t>(af), std::move(bdd)});
    }

    // === 构建（需要 CuddCore 依赖） ===
    void buildIfMissing(Formula* af, CuddCore& core)
    {
        if (!hasBuilt(af))
        {
            record(af, core.newBddVar());
        }
    }

    // === 访问内部数据（用于遍历等） ===
    const std::vector<Formula*>& indexedFormulas() const { return indexed_formulas_; }
    const std::unordered_map<uint64_t, CUDD::BDD>& formulaToBddMap() const { return formula_to_bdd_; }
};

}  // namespace Cosy
