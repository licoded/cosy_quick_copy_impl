#include "formula/builder.hpp"
#include "formula/formula.hpp"
#include "formula/synthesis_context.hpp"
#include "ltlparser/trans.h"
#include <set>
#include <stdexcept>

namespace Cosy {

// ========== Hash Consing Implementation ==========

bool FormulaBuilder::FormulaEqual::operator()(const Formula* a, const Formula* b) const noexcept {
    // Fast path: same pointer
    if (a == b) return true;

    // One is null, other is not
    if (!a || !b) return false;

    // Structural comparison
    return a->hash() == b->hash() &&
           a->op_ == b->op_ &&
           a->left_ == b->left_ &&
           a->right_ == b->right_ &&
           a->var_id_ == b->var_id_;
}

size_t FormulaBuilder::compute_hash(Operator op, Formula* left, Formula* right, unsigned int var_id) {
    // Hash combination using boost::hash_combine style
    size_t h = static_cast<size_t>(op);

    if (left) {
        h ^= left->hash() + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
    if (right) {
        h ^= right->hash() + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
    if (op == Operator::Literal) {
        h ^= static_cast<size_t>(var_id) + 0x9e3779b9 + (h << 6) + (h >> 2);
    }

    return h;
}

Formula* FormulaBuilder::make_binary_raw(Operator op, Formula* left, Formula* right) {
    // Raw binary creation without canonicalization (used internally by rebuild_chain)
    size_t hash = compute_hash(op, left, right, 0);
    Formula key(op, left, right, 0, hash, &context_);

    auto it = unique_table_.find(&key);
    if (it != unique_table_.end()) {
        return *it;
    }

    Formula* new_formula = context_.create_formula(op, left, right, 0, hash);
    unique_table_.insert(new_formula);
    return new_formula;
}

// ========== Canonicalization Helpers ==========

void FormulaBuilder::collect_terms(Formula* f, std::set<Formula*>& terms, Operator op) {
    if (!f) return;

    if (f->op() == op) {
        // Flatten chain: collect both sides
        collect_terms(f->left(), terms, op);
        collect_terms(f->right(), terms, op);
    } else {
        // Leaf term
        terms.insert(f);
    }
}

Formula* FormulaBuilder::rebuild_chain(const std::set<Formula*>& terms, Operator op) {
    if (terms.empty()) {
        // Identity element
        return (op == Operator::And) ? make_true() : make_false();
    }

    if (terms.size() == 1) {
        return *terms.begin();
    }

    // Build right-leaning chain
    auto it = terms.begin();
    Formula* result = *it;
    ++it;

    for (; it != terms.end(); ++it) {
        // Use make_binary_raw to avoid re-canonicalization
        result = make_binary_raw(op, result, *it);
    }

    return result;
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
    size_t hash = compute_hash(Operator::True, nullptr, nullptr, 0);
    true_formula_ = context_.create_formula(Operator::True, nullptr, nullptr, 0, hash);
    unique_table_.insert(true_formula_);
    return true_formula_;
}

Formula* FormulaBuilder::make_false() {
    // Return singleton if already created
    if (false_formula_) {
        return false_formula_;
    }

    // Create new False formula
    size_t hash = compute_hash(Operator::False, nullptr, nullptr, 0);
    false_formula_ = context_.create_formula(Operator::False, nullptr, nullptr, 0, hash);
    unique_table_.insert(false_formula_);
    return false_formula_;
}

Formula* FormulaBuilder::make_literal(const std::string& var_name) {
    unsigned int id = context_.symbols().get_or_create_variable_id(var_name);

    // Compute hash
    size_t hash = compute_hash(Operator::Literal, nullptr, nullptr, id);

    // Create temporary key for lookup
    Formula key(Operator::Literal, nullptr, nullptr, id, hash, &context_);

    // Search in unique table
    auto it = unique_table_.find(&key);
    if (it != unique_table_.end()) {
        return *it; // Found existing
    }

    // Not found, create new
    Formula* new_formula = context_.create_formula(Operator::Literal, nullptr, nullptr, id, hash);
    unique_table_.insert(new_formula);
    return new_formula;
}

Formula* FormulaBuilder::make_unary(Operator op, Formula* sub_formula) {
    if (!is_unary_operator(op)) {
        throw std::invalid_argument("Invalid unary operator");
    }

    // Compute hash
    size_t hash = compute_hash(op, nullptr, sub_formula, 0);

    // Create temporary key for lookup
    Formula key(op, nullptr, sub_formula, 0, hash, &context_);

    // Search in unique table
    auto it = unique_table_.find(&key);
    if (it != unique_table_.end()) {
        return *it; // Found existing
    }

    // Not found, create new
    Formula* new_formula = context_.create_formula(op, nullptr, sub_formula, 0, hash);
    unique_table_.insert(new_formula);
    return new_formula;
}

Formula* FormulaBuilder::make_binary(Operator op, Formula* left, Formula* right) {
    if (!is_binary_operator(op)) {
        throw std::invalid_argument("Invalid binary operator");
    }

    // Apply canonicalization for And/Or operators
    if (op == Operator::And || op == Operator::Or) {
        std::set<Formula*> terms;

        // Phase 1: Flatten nested structures
        collect_terms(left, terms, op);
        collect_terms(right, terms, op);

        // Phase 2: Apply simplification rules
        if (op == Operator::And) {
            // Check for False (dominance: False & anything → False)
            Formula* false_f = make_false();
            if (terms.find(false_f) != terms.end()) {
                return false_f;
            }

            // Remove True (identity: True & x → x)
            Formula* true_f = make_true();
            terms.erase(true_f);

            // TODO: Check for complementary literals (a & !a → False)
        } else { // Or
            // Check for True (dominance: True | anything → True)
            Formula* true_f = make_true();
            if (terms.find(true_f) != terms.end()) {
                return true_f;
            }

            // Remove False (identity: False | x → x)
            Formula* false_f = make_false();
            terms.erase(false_f);

            // TODO: Check for complementary literals (a | !a → True)
        }

        // Phase 3: Rebuild canonicalized chain
        return rebuild_chain(terms, op);
    }

    // TODO: Add canonicalization for Until/Release operators

    // For other operators, use standard hash consing
    size_t hash = compute_hash(op, left, right, 0);
    Formula key(op, left, right, 0, hash, &context_);

    auto it = unique_table_.find(&key);
    if (it != unique_table_.end()) {
        return *it;
    }

    Formula* new_formula = context_.create_formula(op, left, right, 0, hash);
    unique_table_.insert(new_formula);
    return new_formula;
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
