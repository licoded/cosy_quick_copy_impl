#pragma once

#include "operator.hpp"
#include <memory>
#include <string>
#include <vector>

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

    // Formula生命周期管理（供FormulaBuilder使用）
    Formula* create_formula(Operator op, Formula* left, Formula* right, unsigned int var_id, size_t hash);

private:
    SymbolTable* symbols_;
    FormulaBuilder* formula_builder_;
    std::vector<std::unique_ptr<Formula>> formulas_;
};

} // namespace Cosy
