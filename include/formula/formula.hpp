#pragma once

#include "operator.hpp"
#include <string>
#include <unordered_set>

namespace Cosy {

class FormulaBuilder;
class FormulaStringifier;
class SynthesisContext;
class FormulaHasher;
class Visitor;

class Formula {
public:
    Formula() = delete;
    ~Formula();

    std::string toString() const;
    bool is_binary() const;
    size_t hash() const { return hash_; }
    Formula* simplify();
    Formula* nnf();
    Formula* xnf();
    Formula* progression(const std::unordered_set<int>& literals);

    // 访问者模式支持
    void accept(Visitor& visitor) const;

    // Accessors for canonicalization
    Operator op() const { return op_; }
    Formula* left() const { return left_; }
    Formula* right() const { return right_; }
    unsigned int var_id() const { return var_id_; }
    const std::string& var_name() const { return get_global_symbol_table().get_var_name(var_id_); }

private:
    friend class FormulaBuilder;
    friend class FormulaStringifier;
    friend class SynthesisContext;
    friend class FormulaHasher;

    Formula(Operator op, Formula* left, Formula* right, unsigned int var_id, SynthesisContext* context);

    SynthesisContext* context_;
    Operator op_ = Operator::Undefined;
    Formula* left_ = nullptr;
    Formula* right_ = nullptr;
    unsigned int var_id_ = 0;
    size_t hash_ = 0;
};

} // namespace Cosy
