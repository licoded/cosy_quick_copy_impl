#pragma once

#include "operator.hpp"
#include <string>

namespace Cosy {

class FormulaBuilder;

class Formula {
public:
    Formula() = delete;
    Formula(const std::string& str);
    ~Formula();

    std::string toString() const;
    bool is_binary() const;

private:
    friend class FormulaBuilder;

    Formula(Operator op, Formula* left, Formula* right, unsigned int var_id = 0);

    Operator op_ = Operator::Undefined;
    Formula* left_ = nullptr;
    Formula* right_ = nullptr;
    unsigned int var_id_ = 0;
};

} // namespace Cosy
