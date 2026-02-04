#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace Cosy {

enum class Operator {
    True,
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

class SymbolTable {
public:
    void initialize_operators();
    unsigned int get_or_create_variable_id(const std::string& var_name);
    const std::string& get_name(unsigned int id) const;

private:
    std::vector<std::string> names_;
    std::unordered_map<std::string, int> ids_;
};

} // namespace Cosy
