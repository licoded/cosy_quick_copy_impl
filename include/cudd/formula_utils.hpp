#pragma once

#include "formula/formula.hpp"
#include "formula/builder.hpp"
#include <string>
#include <unordered_set>
#include <vector>

namespace Cosy {

/**
 * @brief 检查公式是否为 TAIL
 * @param af 要检查的公式
 * @return 如果公式是 TAIL 则返回 true
 */
bool is_tail(const Formula* af);

/**
 * @brief 检查公式是否为 NOT_TAIL
 * @param af 要检查的公式
 * @return 如果公式是 NOT_TAIL 则返回 true
 */
bool is_not_tail(const Formula* af);

/**
 * @brief 将多个公式合取组合成一个公式
 * @param builder FormulaBuilder 引用
 * @param formulas 公式指针列表
 * @return 合取后的公式，如果列表为空则返回 true
 */
Formula* formula_conjunction(FormulaBuilder& builder, const std::vector<Formula*>& formulas);

/**
 * @brief 收集公式中的变量 ID 到给定的集合中
 * @param af 公式指针
 * @param var_set 输出的变量 ID 集合
 */
void collect_var_ids(const Formula* af, std::unordered_set<int>& var_set);

} // namespace Cosy
