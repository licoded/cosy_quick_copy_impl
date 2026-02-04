#pragma once

#include "operator.hpp"
#include <string>

namespace Cosy {

class Formula;

class FormulaBuilder {
public:
    static Formula* parse(const std::string& str);
    static Formula* make_true();
    static Formula* make_false();
    static Formula* make_literal(const std::string& name);
    static Formula* make_unary(Operator op, Formula* operand);
    static Formula* make_binary(Operator op, Formula* left, Formula* right);

private:
    static SymbolTable& get_symbol_table();
};

} // namespace Cosy
