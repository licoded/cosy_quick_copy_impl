#pragma once

#include <string>

namespace Cosy {

class Formula;
class FormulaBuilder;
class SymbolTable;

class SynthesisContext {
public:
    SynthesisContext();
    ~SynthesisContext();

    // 访问器
    FormulaBuilder& formula_builder();
    SymbolTable& symbols();

    // 便捷方法
    Formula* parse_formula(const std::string& str);

private:
    SymbolTable* symbols_;
    FormulaBuilder* formula_builder_;
};

} // namespace Cosy
