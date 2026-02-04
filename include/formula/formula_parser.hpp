#pragma once

#include "ltlparser/ltl_formula.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <vector>
#include <unordered_map>

namespace Cosy {

class Formula {
public:
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

    Formula();
    Formula(const char* input);
    ~Formula();

    std::string toString() const;

private:
    Formula(const ltl_formula* formula, bool is_not = false);
    void build(const ltl_formula* formula, bool is_not = false);
    void build_atom(const char* name, bool is_not = false);

    Operator op_ = Operator::Undefined;
    Formula* left_ = nullptr;
    Formula* right_ = nullptr;

    std::string error_message_;

    static std::vector<std::string> names_;
    static std::unordered_map<std::string, int> ids_;

    Formula* parse(const char* input) {
        try {
            return new Formula(input);
        } catch (const std::exception& e) {
            error_message_ = e.what();
            return nullptr;
        }
    }
    const std::string& error() const { return error_message_; }
};

} // namespace Cosy

