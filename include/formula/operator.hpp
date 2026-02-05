#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace Cosy {

enum class Operator {
    True = 0, // needed in operator_info_table
    False,
    Literal,
    Not,
    Or,
    And,
    Next,
    WNext,
    Until,
    Release,
    Undefined
};

enum class OperatorArity {
    Nullary,  // true, false, literal (0 operands)
    Unary,    // !, X, X[!] (1 operand)
    Binary    // &, |, U, R (2 operands)
};

struct OperatorInfo {
    const char* name;
    OperatorArity arity;
};

// Get operator metadata
const OperatorInfo& get_operator_info(Operator op);

// Helper functions for operator validation
bool is_unary_operator(Operator op);
bool is_binary_operator(Operator op);

class SymbolTable {
public:
    unsigned int get_or_create_variable_id(const std::string& var_name);
    const std::string& get_name(unsigned int id) const;
    static const char *get_op_str(Operator op);

private:
    std::vector<std::string> names_;
    std::unordered_map<std::string, int> ids_;
};

// Global symbol table accessor
SymbolTable& get_global_symbol_table();

} // namespace Cosy
