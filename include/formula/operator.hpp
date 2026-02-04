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

// Operator name registry functions
void initialize_operator_names(std::vector<std::string>& names);
unsigned int register_variable_name(const std::string& var_name,
                                     std::vector<std::string>& names,
                                     std::unordered_map<std::string, int>& ids);

} // namespace Cosy
