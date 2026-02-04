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
    Formula(const ltl_formula* formula);
    void build(const ltl_formula* formula);
    void build_atom(const char* name);

    Operator op_ = Operator::Undefined;
    Formula* left_ = nullptr;
    Formula* right_ = nullptr;

    bool is_binary() const;

    static std::vector<std::string> names_;
    static std::unordered_map<std::string, int> ids_;
};

} // namespace Cosy

