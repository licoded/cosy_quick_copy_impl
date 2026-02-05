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
    size_t hash() const { return hash_; }

    // Accessors for canonicalization
    Operator op() const { return op_; }
    Formula* left() const { return left_; }
    Formula* right() const { return right_; }
    unsigned int var_id() const { return var_id_; }

private:
    friend class FormulaBuilder;
    friend class FormulaStringifier;
    friend class SynthesisContext;

    Formula(Operator op, Formula* left, Formula* right, unsigned int var_id, size_t hash, SynthesisContext* context);

    SynthesisContext* context_;
    Operator op_ = Operator::Undefined;
    Formula* left_ = nullptr;
    Formula* right_ = nullptr;
    unsigned int var_id_ = 0;
    size_t hash_ = 0;
};

} // namespace Cosy
