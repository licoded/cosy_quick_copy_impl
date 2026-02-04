#pragma once

#include <string>

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

} // namespace Cosy
