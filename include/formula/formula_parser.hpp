#pragma once

#include "ltlparser/ltl_formula.h"
#include <string>
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
        WNext, // weak Next, for LTLf
        Until,
        Release,
        Undefined
    };

    Formula();
    Formula(const char* input, bool is_ltlf = false);
    Formula(const ltl_formula* formula, bool is_not = false, bool is_ltlf = false);
    ~Formula();

    std::string toString() const;

private:
    void build(const ltl_formula* formula, bool is_not = false, bool is_ltlf = false);
    void build_atom(const char* name, bool is_not = false);

    Operator op_ = Operator::Undefined;
    Formula* left_ = nullptr;
    Formula* right_ = nullptr;

    static std::vector<std::string> names_;
    static std::unordered_map<std::string, int> ids_;
};

} // namespace Cosy

