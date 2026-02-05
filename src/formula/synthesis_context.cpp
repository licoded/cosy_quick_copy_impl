#include "formula/synthesis_context.hpp"
#include "formula/builder.hpp"
#include "formula/formula.hpp"
#include "formula/operator.hpp"

namespace Cosy {

SynthesisContext::SynthesisContext() {
    symbols_ = new SymbolTable();
    formula_builder_ = new FormulaBuilder(*this);
}

SynthesisContext::~SynthesisContext() {
    delete formula_builder_;
    delete symbols_;
}

FormulaBuilder& SynthesisContext::formula_builder() {
    return *formula_builder_;
}

SymbolTable& SynthesisContext::symbols() {
    return *symbols_;
}

Formula* SynthesisContext::parse_formula(const std::string& str) {
    return formula_builder_->parse(str);
}

Formula* SynthesisContext::create_formula(Operator op, Formula* left, Formula* right, unsigned int var_id) {
    Formula* formula = new Formula(op, left, right, var_id, this);
    formulas_.emplace_back(formula);
    return formula;
}

} // namespace Cosy
