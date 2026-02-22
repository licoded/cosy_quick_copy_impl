#include "formula/formula.hpp"
#include "formula/builder.hpp"
#include <cassert>
#include <unordered_map>

namespace Cosy {

std::unordered_map<Operator, Operator> DeMorganMap = {
    {Operator::And, Operator::Or},
    {Operator::Or, Operator::And},
    {Operator::Next, Operator::WNext},
    {Operator::WNext, Operator::Next},
    {Operator::Until, Operator::Release},
    {Operator::Release, Operator::Until}
};

class NNFTransformer {
public:
    static Formula* to_nnf(Formula* formula, FormulaBuilder& builder) {
        if (!formula) return nullptr;

        const Operator op = formula->op();
        switch (op) {
            case Operator::Not:
                return to_nnf_not(formula->right(), builder);
            default:
                Formula* left_nnf = to_nnf(formula->left(), builder);
                Formula* right_nnf = to_nnf(formula->right(), builder);
                return builder.make_binary(op, left_nnf, right_nnf);
        }
    }

    static Formula* to_nnf_not(Formula* formula, FormulaBuilder& builder) {
        if (!formula) return nullptr;

        const Operator op = formula->op();
        switch (op) {
            case Operator::True:
                return builder.make_false();
            case Operator::False:
                return builder.make_true();
            case Operator::Literal:
                return builder.make_literal(-formula->var_id());
            case Operator::Not:
                return to_nnf(formula->right(), builder);
            default:
                assert(DeMorganMap.count(op) && "Operator must have a De Morgan dual");
                const Operator de_morgan_op = DeMorganMap.at(op);
                Formula* left_nnf = to_nnf_not(formula->left(), builder);
                Formula* right_nnf = to_nnf_not(formula->right(), builder);
                return builder.make_binary(de_morgan_op, left_nnf, right_nnf);
        }
    }
};

} // namespace Cosy