#pragma once

#include "formula/formula.hpp"
#include "formula/builder.hpp"
#include <cassert>
#include <unordered_map>

namespace Cosy {

class XNFTransformer {
public:
    static Formula* to_xnf(Formula* formula, FormulaBuilder& builder) {
        if (formula == nullptr) {
            return nullptr;
        }

        // 缓存检查
        auto it = cache_.find(formula);
        if (it != cache_.end()) {
            return it->second;
        }

        Formula* result = nullptr;
        Operator op = formula->op();

        switch (op) {
        case Operator::True:
        case Operator::False:
        case Operator::Literal:
        case Operator::Not:
        case Operator::Next:
        case Operator::WNext:
            // 这些不需要转换
            result = formula;
            break;

        case Operator::And:
        case Operator::Or: {
            Formula* left_xnf = to_xnf(formula->left(), builder);
            Formula* right_xnf = to_xnf(formula->right(), builder);
            result = builder.formula_reduce(op, {left_xnf, right_xnf}, true);
            break;
        }

        case Operator::Until: {
            // l U r = [xnf(r) & not_tail] | [xnf(l) & X(l U r)]
            // 不可以不转移直接结束 | 左边有 not_tail, 右边有 X(l U r)
            // 转移的话 要么左边满足 xnf(r) 要么右边满足 xnf(l) & X(l U r)
            Formula* left_xnf = to_xnf(formula->left(), builder);
            Formula* right_xnf = to_xnf(formula->right(), builder);

            Formula* not_tail = builder.make_not_tail();
            Formula* next_phi = builder.make_unary(Operator::Next, formula);

            // xnf(r) & not_tail
            Formula* right_and_not_tail = builder.formula_reduce(Operator::And, {right_xnf, not_tail}, true);
            // xnf(l) & X(l U r)
            Formula* left_and_next = builder.formula_reduce(Operator::And, {left_xnf, next_phi}, true);
            // 组合
            result = builder.formula_reduce(Operator::Or, {right_and_not_tail, left_and_next}, true);
            break;
        }

        case Operator::Release: {
            // l R r = [xnf(r) | tail] & [xnf(l) | WX(l R r)]
            // 可以不转移直接结束 & 左边取 tail, 右边取 WX(l R r)
            // 转移的话 & 左边必须取 xnf(r), 右边要么满足 xnf(l) 要么满足 WX(l R r)
            Formula* left_xnf = to_xnf(formula->left(), builder);
            Formula* right_xnf = to_xnf(formula->right(), builder);

            Formula* tail = builder.make_tail();
            Formula* wnext_phi = builder.make_unary(Operator::WNext, formula);

            // xnf(r) | tail
            Formula* right_or_tail = builder.formula_reduce(Operator::Or, {right_xnf, tail}, true);
            // xnf(l) | WX(l R r)
            Formula* left_or_wnext = builder.formula_reduce(Operator::Or, {left_xnf, wnext_phi}, true);
            // 组合
            result = builder.formula_reduce(Operator::And, {right_or_tail, left_or_wnext}, true);
            break;
        }

        default:
            assert(false && "Unknown operator");
            break;
        }

        cache_[formula] = result;
        return result;
    }

    static void clear_cache() {
        cache_.clear();
    }

private:
    static std::unordered_map<Formula*, Formula*> cache_;
};

} // namespace Cosy
