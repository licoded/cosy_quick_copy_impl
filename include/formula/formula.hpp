#pragma once

#include "operator.hpp"
#include <string>

namespace Cosy {

class FormulaBuilder;
class FormulaStringifier;
class SynthesisContext;

class Formula {
public:
    Formula() = delete;
    ~Formula();

    std::string toString() const;
    bool is_binary() const;

private:
    friend class FormulaBuilder;
    friend class FormulaStringifier;

    Formula(Operator op, Formula* left, Formula* right, unsigned int var_id, SynthesisContext* context);

    SynthesisContext* context_;
    Operator op_ = Operator::Undefined;
    Formula* left_ = nullptr;
    Formula* right_ = nullptr;
    unsigned int var_id_ = 0;
};

} // namespace Cosy
