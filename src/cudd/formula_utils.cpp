#include "cudd/formula_utils.hpp"
#include "cudd/cudd_config.hpp"
#include "formula/operator.hpp"
#include "formula/variable_collector.hpp"
#include <spdlog/spdlog.h>
#include <cstdlib>

namespace Cosy {

[[noreturn]] void exit_with_error(const std::string& msg) {
    spdlog::error("{}", msg);
    std::exit(1);
}

// tail = true R false
bool is_tail(const Formula* af) {
    if (af == nullptr) {
        return false;
    }
    if (af->op() != Operator::Release) {
        return false;
    }
    // Check if left is true and right is false
    const Formula* left = af->left();
    const Formula* right = af->right();
    return left != nullptr && right != nullptr &&
           left->op() == Operator::True && right->op() == Operator::False;
}

// not_tail = true U true
bool is_not_tail(const Formula* af) {
    if (af == nullptr) {
        return false;
    }
    if (af->op() != Operator::Until) {
        return false;
    }
    // Check if both left and right are true
    const Formula* left = af->left();
    const Formula* right = af->right();
    return left != nullptr && right != nullptr &&
           left->op() == Operator::True && right->op() == Operator::True;
}

Formula* formula_conjunction(FormulaBuilder& builder, const std::vector<Formula*>& formulas) {
    if (formulas.empty()) {
        return builder.make_true();
    }

    Formula* result = formulas[0];
    for (size_t i = 1; i < formulas.size(); ++i) {
        result = builder.make_binary(Operator::And, result, formulas[i]);
    }
    return result;
}

void collect_var_ids(const Formula* af, std::unordered_set<int>& var_set) {
    if (af == nullptr) {
        return;
    }

    // 使用 VariableCollector 收集变量
    auto var_ids = collect_variables(const_cast<Formula*>(af));
    for (auto id : var_ids) {
        var_set.insert(static_cast<int>(id));
    }
}

} // namespace Cosy
