#include "formula/builder.hpp"
#include "formula/formula.hpp"
#include "formula/synthesis_context.hpp"
#include "formula/hash.hpp"
#include "ltlparser/trans.h"
#include <stdexcept>

namespace Cosy {

// ========== Hash Consing Implementation ==========

bool FormulaBuilder::FormulaEqual::operator()(const Formula* a, const Formula* b) const noexcept {
    // Fast path: same pointer
    if (a == b) return true;

    // One is null, other is not
    if (!a || !b) return false;

    // Must have same operator and hash
    if (a->op_ != b->op_ || a->hash() != b->hash()) {
        return false;
    }

    // For commutative operators (And, Or), normalize order before comparison
    if (a->op_ == Operator::And || a->op_ == Operator::Or) {
        Formula* a_left = a->left_;
        Formula* a_right = a->right_;
        Formula* b_left = b->left_;
        Formula* b_right = b->right_;

        // Normalize a's children
        if (a_left && a_right && a_left > a_right) {
            std::swap(a_left, a_right);
        }

        // Normalize b's children
        if (b_left && b_right && b_left > b_right) {
            std::swap(b_left, b_right);
        }

        // Compare normalized children
        return a_left == b_left && a_right == b_right;
    }

    // For non-commutative operators, strict structural comparison
    return a->left_ == b->left_ &&
           a->right_ == b->right_ &&
           a->var_id_ == b->var_id_;
}

// ========== Public API ==========

namespace {
    // Forward declaration for parser
    Formula* build_formula(FormulaBuilder& builder, const ltl_formula* ast);
}

FormulaBuilder::FormulaBuilder(SynthesisContext& context)
    : context_(context) {}

Formula* FormulaBuilder::parse(const std::string& input) {
    if (input.empty()) {
        throw std::invalid_argument("Input formula cannot be empty");
    }
    ltl_formula* ast = getAST(input.c_str());
    if (ast == nullptr) {
        throw std::runtime_error("Failed to parse formula");
    }
    Formula* formula = build_formula(*this, ast);
    destroy_formula(ast);
    return formula;
}

Formula* FormulaBuilder::make_true() {
    // Return singleton if already created
    if (true_formula_) {
        return true_formula_;
    }

    // Create new True formula
    true_formula_ = context_.create_formula(Operator::True, nullptr, nullptr, 0);
    unique_table_.insert(true_formula_);
    return true_formula_;
}

Formula* FormulaBuilder::make_false() {
    // Return singleton if already created
    if (false_formula_) {
        return false_formula_;
    }

    // Create new False formula
    false_formula_ = context_.create_formula(Operator::False, nullptr, nullptr, 0);
    unique_table_.insert(false_formula_);
    return false_formula_;
}

Formula* FormulaBuilder::make_tail() {
    // Return singleton if already created
    if (tail_formula_) {
        return tail_formula_;
    }

    // Create new Tail formula
    tail_formula_ = make_binary(Operator::Release, make_false(), make_false());
    return tail_formula_;
}

Formula* FormulaBuilder::make_not_tail() {
    // Return singleton if already created
    if (not_tail_formula_) {
        return not_tail_formula_;
    }

    // Create new NOT_Tail formula
    not_tail_formula_ = make_binary(Operator::Until, make_true(), make_true());
    return not_tail_formula_;
}

Formula* FormulaBuilder::make_literal(const std::string& var_name) {
    unsigned int id = context_.symbols().get_or_create_variable_id(var_name);
    return make_literal(id);
}

Formula* FormulaBuilder::make_literal(int var_id) {
    if (var_id < 0) {
        // Negative var_id means negated literal
        Formula* positive_literal = make_literal(static_cast<unsigned int>(-var_id));
        return make_unary(Operator::Not, positive_literal);
    } else {
        return make_literal(static_cast<unsigned int>(var_id));
    }
}

Formula* FormulaBuilder::make_literal(unsigned int var_id) {
    // Create temporary key for lookup (will compute hash automatically)
    Formula key(Operator::Literal, nullptr, nullptr, var_id, &context_);

    // Search in unique table
    auto it = unique_table_.find(&key);
    if (it != unique_table_.end()) {
        return *it; // Found existing
    }

    // Not found, create new
    Formula* new_formula = context_.create_formula(Operator::Literal, nullptr, nullptr, var_id);
    unique_table_.insert(new_formula);
    return new_formula;
}

Formula* FormulaBuilder::make_unary(Operator op, Formula* sub_formula) {
    if (!is_unary_operator(op)) {
        throw std::invalid_argument("Invalid unary operator");
    }

    // Create temporary key for lookup (will compute hash automatically)
    Formula key(op, nullptr, sub_formula, 0, &context_);

    // Search in unique table
    auto it = unique_table_.find(&key);
    if (it != unique_table_.end()) {
        return *it; // Found existing
    }

    // Not found, create new
    Formula* new_formula = context_.create_formula(op, nullptr, sub_formula, 0);
    unique_table_.insert(new_formula);
    return new_formula;
}

Formula* FormulaBuilder::make_binary(Operator op, Formula* left, Formula* right) {
    if (!is_binary_operator(op)) {
        throw std::invalid_argument("Invalid binary operator");
    }

    // Create temporary key for lookup (will compute hash automatically)
    Formula key(op, left, right, 0, &context_);

    auto it = unique_table_.find(&key);
    if (it != unique_table_.end()) {
        return *it;
    }

    Formula* new_formula = context_.create_formula(op, left, right, 0);
    unique_table_.insert(new_formula);
    return new_formula;
}

Formula* FormulaBuilder::make_global(Formula* operand) {
    return make_binary(Operator::Release, make_false(), operand);
}

Formula* FormulaBuilder::make_future(Formula* operand) {
    return make_binary(Operator::Until, make_true(), operand);
}

Formula* FormulaBuilder::formula_reduce(Operator op, const std::vector<Formula*>& formulas) {
    if (formulas.empty()) {
        return op == Operator::And ? make_true() : make_false();
    }
    if (formulas.size() == 1) {
        return formulas[0];
    }
    Formula* result = formulas[0];
    for (size_t i = 1; i < formulas.size(); ++i) {
        result = make_binary(op, result, formulas[i]);
    }
    return result;
}

Formula* FormulaBuilder::make_ands(const std::vector<Formula*>& formulas) {
    return formula_reduce(Operator::And, formulas);
}

Formula* FormulaBuilder::make_ors(const std::vector<Formula*>& formulas) {
    return formula_reduce(Operator::Or, formulas);
}

namespace {

Formula* build_formula(FormulaBuilder& builder, const ltl_formula* ast) {
    if (ast == nullptr) {
        throw std::invalid_argument("AST node cannot be null");
    }

    Formula *left = ast->_left == nullptr ? nullptr : build_formula(builder, ast->_left);
    Formula *right = ast->_right == nullptr ? nullptr : build_formula(builder, ast->_right);

    switch (ast->_type) {
        case eTRUE:
            return builder.make_true();
        case eFALSE:
            return builder.make_false();
        case eLITERAL:
            return builder.make_literal(ast->_var);
        case eNOT:
            return builder.make_unary(Operator::Not, right);
        case eNEXT:
            return builder.make_unary(Operator::Next, right);
        case eWNEXT:
            return builder.make_unary(Operator::WNext, right);
        case eGLOBALLY:
            return builder.make_binary(Operator::Release, builder.make_false(), right);
        case eFUTURE:
            return builder.make_binary(Operator::Until, builder.make_true(), right);
        case eUNTIL:
            return builder.make_binary(Operator::Until, left, right);
        case eRELEASE:
            return builder.make_binary(Operator::Release, builder.make_false(), right);
        case eAND:
            return builder.make_binary(Operator::And, left, right);
        case eOR:
            return builder.make_binary(Operator::Or, left, right);
        case eIMPLIES: {
            Formula* not_left = builder.make_unary(Operator::Not, left);
            Formula* result = builder.make_binary(Operator::Or, not_left, right);
            return result;
        }
        case eEQUIV: {
            Formula* not_left = builder.make_unary(Operator::Not, left);
            Formula* not_right = builder.make_unary(Operator::Not, right);
            Formula* left_implies_right = builder.make_binary(Operator::Or, not_left, right);
            Formula* right_implies_left = builder.make_binary(Operator::Or, not_right, left);
            Formula* result = builder.make_binary(Operator::And, left_implies_right, right_implies_left);
            return result;
        }
        default:
            throw std::runtime_error("The formula cannot be recognized!");
    }
}

} // anonymous namespace

} // namespace Cosy
