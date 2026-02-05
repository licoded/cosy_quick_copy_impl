#include "formula/synthesis_context.hpp"
#include "formula/builder.hpp"
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

} // namespace Cosy
