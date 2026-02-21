#pragma once

#include "formula.hpp"
#include "visitor.hpp"
#include <unordered_set>

namespace Cosy {

/**
 * @brief 用于收集公式中所有变量 ID 的 Visitor
 *
 * 遍历公式树，收集所有 Literal 节点的 var_id
 */
class VariableCollector : public Visitor {
public:
    VariableCollector() : var_ids_() {}

    void visit(Formula* formula) override;
    void visit_binary(Formula* formula) override;
    void visit_unary(Formula* formula) override;
    void visit_literal(Formula* formula) override;
    void visit_constant(Formula* formula) override;

    void visit_and(Formula* formula) override;
    void visit_or(Formula* formula) override;
    void visit_not(Formula* formula) override;
    void visit_next(Formula* formula) override;
    void visit_wnext(Formula* formula) override;
    void visit_until(Formula* formula) override;
    void visit_release(Formula* formula) override;

    void visit_true(Formula* formula) override;
    void visit_false(Formula* formula) override;

    // 获取收集到的变量 ID 集合
    const std::unordered_set<unsigned int>& get_var_ids() const { return var_ids_; }

    // 清空收集结果
    void clear() { var_ids_.clear(); }

private:
    std::unordered_set<unsigned int> var_ids_;
};

/**
 * @brief 便捷函数：收集公式中的所有变量 ID
 * @param formula 要遍历的公式
 * @return 变量 ID 的集合
 */
std::unordered_set<unsigned int> collect_variables(Formula* formula);

} // namespace Cosy
