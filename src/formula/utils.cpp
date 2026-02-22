#include "formula/utils.hpp"
#include "cudd/cudd_config.hpp"
#include "formula/operator.hpp"
#include <spdlog/spdlog.h>
#include <cstdlib>

namespace Cosy {

[[noreturn]] void exit_with_error(const std::string& msg) {
    spdlog::error("{}", msg);
    std::exit(1);
}

// tail = false R false
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
           left->op() == Operator::False && right->op() == Operator::False;
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

void collect_literals(const Formula* af, std::unordered_set<int>& var_set) {
    if (af == nullptr) {
        return;
    }

    switch(af->op()) {
        case Operator::Literal:
            var_set.insert(af->var_id());
            break;
        case Operator::Not:
            assert(af->right() != nullptr && "NOT operator must have a right child");
            assert(af->right()->op() == Operator::Literal && "NOT operator's right child must be a literal");
            var_set.insert(-af->right()->var_id());
            break;
        case Operator::And:
            collect_literals(af->left(), var_set);
            collect_literals(af->right(), var_set);
            break;
        default:
            assert(false && "Unsupported operator in collect_literals: only Literal, Not, and And are allowed");
            break;
    }
}

} // namespace Cosy
