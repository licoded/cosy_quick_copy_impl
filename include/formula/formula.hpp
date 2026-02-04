#pragma once

#include "operator.hpp"
#include <string>

namespace Cosy {

class Formula {
public:
    Formula() = delete;
    Formula(Operator op, Formula* left, Formula* right, unsigned int var_id = 0);
    Formula(const std::string& str);
    static Formula* parse(const std::string& str);
    ~Formula();

    std::string toString() const;
    bool is_binary() const;

    static Formula* make_true();
    static Formula* make_false();
    static Formula* make_literal(const std::string& name);
    static Formula* make_unary(Operator op, Formula* operand);
    static Formula* make_binary(Operator op, Formula* left, Formula* right);

private:
    Operator op_ = Operator::Undefined;
    Formula* left_ = nullptr;
    Formula* right_ = nullptr;
    unsigned int var_id_ = 0;

    static SymbolTable symbol_table_;
};

} // namespace Cosy
