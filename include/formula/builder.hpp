#pragma once

#include "formula.hpp"
#include "operator.hpp"
#include <string>
#include <unordered_set>

namespace Cosy {

class Formula;
class SynthesisContext;

class FormulaBuilder {
public:
    explicit FormulaBuilder(SynthesisContext& context);

    Formula* parse(const std::string& str);
    Formula* make_true();
    Formula* make_false();
    Formula* make_tail();
    Formula* make_not_tail();
    Formula* make_literal(const std::string& name);
    Formula* make_unary(Operator op, Formula* operand);
    Formula* make_binary(Operator op, Formula* left, Formula* right);

private:
    // Hash consing support
    struct FormulaHash {
        size_t operator()(const Formula* f) const noexcept {
            return f ? f->hash() : 0;
        }
    };

    struct FormulaEqual {
        bool operator()(const Formula* a, const Formula* b) const noexcept;
    };

    using UniqueTable = std::unordered_set<Formula*, FormulaHash, FormulaEqual>;

    SynthesisContext& context_;
    UniqueTable unique_table_;
    Formula* true_formula_ = nullptr;
    Formula* false_formula_ = nullptr;
    Formula* tail_formula_ = nullptr;
    Formula* not_tail_formula_ = nullptr;
};

} // namespace Cosy
