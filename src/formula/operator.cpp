#include "formula/operator.hpp"
#include <stdexcept>

namespace Cosy {

namespace {
    SymbolTable global_symbol_table;

    // Operator information table - order must match Operator enum
    constexpr OperatorInfo operator_info_table[] = {
        {"true",      OperatorArity::Nullary},  // True
        {"false",     OperatorArity::Nullary},  // False
        {"Literal",   OperatorArity::Nullary},  // Literal
        {"!",         OperatorArity::Unary},    // Not
        {"|",         OperatorArity::Binary},   // Or
        {"&",         OperatorArity::Binary},   // And
        {"X[!]",      OperatorArity::Unary},    // Next (strong next)
        {"X",         OperatorArity::Unary},    // WNext (weak next)
        {"U",         OperatorArity::Binary},   // Until
        {"R",         OperatorArity::Binary},   // Release
        {"Undefined", OperatorArity::Nullary}   // Undefined
    };
}

const OperatorInfo& get_operator_info(Operator op) {
    return operator_info_table[static_cast<int>(op)];
}

bool is_unary_operator(Operator op) {
    return get_operator_info(op).arity == OperatorArity::Unary;
}

bool is_binary_operator(Operator op) {
    return get_operator_info(op).arity == OperatorArity::Binary;
}

SymbolTable& get_global_symbol_table() {
    return global_symbol_table;
}

unsigned int SymbolTable::get_or_create_variable_id(const std::string& var_name) {
    auto it = ids_.find(var_name);
    if (it == ids_.end()) {
        unsigned int id = names_.size() + 1;
        ids_[var_name] = id;
        names_.push_back(var_name);
        return id;
    } else {
        return it->second;
    }
}

const std::string& SymbolTable::get_var_name(unsigned int id) const {
    if (id > names_.size()) {
        throw std::out_of_range("Invalid symbol ID");
    }
    return names_[id - 1];
}

const char *SymbolTable::get_op_str(Operator op) {
    return get_operator_info(op).name;
}

} // namespace Cosy
