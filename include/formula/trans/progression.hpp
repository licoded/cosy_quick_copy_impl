#pragma once

#include "formula/formula.hpp"
#include "formula/builder.hpp"
#include "formula/utils.hpp"
#include <cassert>
#include <unordered_set>

namespace Cosy {

class ProgressionTransformer {
public:
    static Formula* progression(Formula* formula, FormulaBuilder& builder, std::unordered_set<int>& literals) {
        if (!formula) return nullptr;

        const Operator op = formula->op();
        switch (op) {
            case Operator::True:
                return builder.make_true();
            case Operator::False:
                return builder.make_false();
            case Operator::Literal: {
                int var_id = formula->var_id();
                if (literals.count(var_id)) {
                    return builder.make_true();
                } else if (literals.count(-var_id)) {
                    return builder.make_false();
                } else {
                    assert(false && "Literal must be in the set of literals for progression");
                }
            }
            case Operator::Not: {
                assert(formula->right()->op() == Operator::Literal && "NOT operator must be applied to a literal in NNF");
                Formula* operand_prog = progression(formula->right(), builder, literals);
                if (operand_prog->op() == Operator::True) {
                    return builder.make_false();
                } else if (operand_prog->op() == Operator::False) {
                    return builder.make_true();
                } else {
                    assert(false && "Progression of a NOT literal must be a constant in NNF");
                }
            }
            case Operator::WNext:
            case Operator::Next: {
                return formula->right(); // WNext phi 和 Next phi 的进展都是 phi
            }
            case Operator::And:
            case Operator::Or: {
                Formula* left_prog = progression(formula->left(), builder, literals);
                Formula* right_prog = progression(formula->right(), builder, literals);
                return builder.formula_reduce(op, {left_prog, right_prog}, true);
            }

            default: {
                if (is_tail(formula)) {
                    return builder.make_false(); // tail 的进展是 false
                } else if (is_not_tail(formula)) {
                    return builder.make_true(); // not_tail 的进展是 true
                }
                assert(false && "Unsupported operator in progression transformation");
                return nullptr;
            }
        }
    }
};

} // namespace Cosy
