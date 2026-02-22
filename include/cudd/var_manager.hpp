#pragma once

#include "cudd/part_var.hpp"
#include "formula/builder.hpp"
#include "formula/formula.hpp"
#include <vector>

namespace Cosy {

/**
 * @brief BDD 变量管理器
 *
 * 负责变量分区管理和 Formula 构建。
 * 不涉及 BDD 操作，只提供数据访问。
 */
class BddVarManager
{
protected:
    PartVar part_var_;
    FormulaBuilder& builder_;

public:
    BddVarManager(PartVar part_var, FormulaBuilder& builder)
        : part_var_(std::move(part_var))
        , builder_(builder)
    {
    }

    ~BddVarManager() = default;

    // 禁止拷贝
    BddVarManager(const BddVarManager&) = delete;
    BddVarManager& operator=(const BddVarManager&) = delete;

    // === PartVar 访问 ===
    PartVar& getPartVar() { return part_var_; }
    const PartVar& getPartVar() const { return part_var_; }

    // === FormulaBuilder 访问 ===
    FormulaBuilder& getBuilder() { return builder_; }
    const FormulaBuilder& getBuilder() const { return builder_; }

    // === 变量信息 ===
    auto getAllVarIds() const { return part_var_.getAllVarIds(); }
    size_t getXVarNum() const { return part_var_.getXVarNum(); }
    size_t getYVarNum() const { return part_var_.getYVarNum(); }
    size_t getAllVarNum() const { return part_var_.getAllVarNum(); }

    // === 获取所有原子（Y变量 + X变量顺序） ===
    std::vector<Formula*> getAtoms() const
    {
        std::vector<Formula*> atoms;
        for (unsigned int varId : part_var_.getYVarIds())
        {
            atoms.push_back(builder_.make_literal(static_cast<int>(varId)));
        }
        for (unsigned int varId : part_var_.getXVarIds())
        {
            atoms.push_back(builder_.make_literal(static_cast<int>(varId)));
        }
        return atoms;
    }

    // === 特殊公式 ===
    Formula* makeTail() const { return builder_.make_tail(); }
    Formula* makeNotTail() const { return builder_.make_not_tail(); }
    Formula* makeTrue() const { return builder_.make_true(); }
    Formula* makeFalse() const { return builder_.make_false(); }

    // === XY 分离 ===
    std::pair<Formula*, Formula*>* splitXY(Formula* af)
    {
        return part_var_.split_XY_from_edgeAf(af, builder_);
    }
};

}  // namespace Cosy
