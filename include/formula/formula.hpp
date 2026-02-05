#pragma once

#include "operator.hpp"
#include <string>

namespace Cosy {

class FormulaBuilder;
class FormulaStringifier;
class SynthesisContext;
class FormulaHasher;

class Formula {
public:
    Formula() = delete;
    ~Formula();

    std::string toString() const;
    bool is_binary() const;
    size_t hash() const { return hash_; }
    Formula* simplify();

    // Accessors for canonicalization
    Operator op() const { return op_; }
    Formula* left() const { return left_; }
    Formula* right() const { return right_; }
    unsigned int var_id() const { return var_id_; }

private:
    friend class FormulaBuilder;
    friend class FormulaStringifier;
    friend class SynthesisContext;
    friend class FormulaHasher;

    // Normal constructor - computes hash automatically
    Formula(Operator op, Formula* left, Formula* right, unsigned int var_id, SynthesisContext* context);

    // Private constructor for creating temporary keys in hash consing (does not compute hash)
    Formula(Operator op, Formula* left, Formula* right, unsigned int var_id, size_t hash, SynthesisContext* context);

    SynthesisContext* context_;
    Operator op_ = Operator::Undefined;
    Formula* left_ = nullptr;
    Formula* right_ = nullptr;
    unsigned int var_id_ = 0;
    size_t hash_ = 0;
};

} // namespace Cosy
