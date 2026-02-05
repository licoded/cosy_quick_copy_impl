#pragma once

#include <string>

namespace Cosy {

class Formula;

class FormulaStringifier {
public:
    static std::string to_string(const Formula* formula);

private:
    static std::string parenthesize_if_binary(const Formula* f);
    static std::string format_binary(const Formula* left, const std::string& op,
                                     const Formula* right);
};

} // namespace Cosy
