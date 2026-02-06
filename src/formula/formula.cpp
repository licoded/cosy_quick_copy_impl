#include "formula/formula.hpp"
#include "formula/builder.hpp"
#include "formula/stringifier.hpp"
#include "formula/synthesis_context.hpp"
#include "formula/simplify/simplifier.hpp"
#include "formula/hash.hpp"
#include "formula/visitor.hpp"
#include "formula/simplification_visitor.hpp"

namespace Cosy {

Formula::Formula(Operator op, Formula* left, Formula* right, unsigned int var_id, SynthesisContext* context)
    : context_(context), op_(op), left_(left), right_(right), var_id_(var_id) {
    hash_ = FormulaHasher::compute(op_, left_, right_, var_id_);
}

Formula::~Formula() {
    // Note: left_ and right_ are not deleted here because all Formula objects
    // are owned and managed by SynthesisContext
}

bool Formula::is_binary() const {
    return left_ != nullptr;
}

std::string Formula::toString() const {
    return FormulaStringifier::to_string(this, context_->symbols());
}

Formula* Formula::simplify() {
    return FormulaSimplifier::simplify(this, context_->formula_builder());
}

void Formula::accept(Visitor& visitor) const {
    visitor.visit(const_cast<Formula*>(this));
}

} // namespace Cosy

