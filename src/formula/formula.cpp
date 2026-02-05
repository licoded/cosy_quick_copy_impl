#include "formula/formula.hpp"
#include "formula/builder.hpp"
#include "formula/stringifier.hpp"
#include "formula/synthesis_context.hpp"

namespace Cosy {

Formula::Formula(Operator op, Formula* left, Formula* right, unsigned int var_id, SynthesisContext* context)
    : context_(context), op_(op), left_(left), right_(right), var_id_(var_id) {}

Formula::~Formula() {
    delete left_;
    delete right_;
}

bool Formula::is_binary() const {
    return left_ != nullptr;
}

std::string Formula::toString() const {
    return FormulaStringifier::to_string(this, context_->symbols());
}

} // namespace Cosy

