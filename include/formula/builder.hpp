#pragma once

#include "operator.hpp"
#include <string>

namespace Cosy {

class Formula;
class SynthesisContext;

class FormulaBuilder {
public:
    explicit FormulaBuilder(SynthesisContext& context);

    Formula* parse(const std::string& str);
    Formula* make_true();
    Formula* make_false();
    Formula* make_literal(const std::string& name);
    Formula* make_unary(Operator op, Formula* operand);
    Formula* make_binary(Operator op, Formula* left, Formula* right);

private:
    SynthesisContext& context_;
};

} // namespace Cosy
