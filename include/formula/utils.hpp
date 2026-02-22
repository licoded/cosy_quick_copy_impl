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
 * @brief 
 * @param af 公式指针，要求是literals CNF
 * @param var_set 输出的变量 ID 集合
 */
void collect_literals(const Formula* af, std::unordered_set<int>& var_set);

} // namespace Cosy
