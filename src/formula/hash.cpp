#include "formula/hash.hpp"
#include "formula/formula.hpp"

namespace Cosy {

size_t FormulaHasher::compute(Operator op, Formula* left, Formula* right, unsigned int var_id) {
    // Normalize order for commutative operators (And, Or)
    // This ensures a & b and b & a have the same hash
    Formula* left_normalized = left;
    Formula* right_normalized = right;

    if ((op == Operator::And || op == Operator::Or) &&
        left_normalized && right_normalized &&
        left_normalized > right_normalized) {
        std::swap(left_normalized, right_normalized);
    }

    size_t h = 0;

    hash_combine(h, static_cast<size_t>(op));

    if (left_normalized)  hash_combine(h, left_normalized->hash());
    if (right_normalized) hash_combine(h, right_normalized->hash());

    if (op == Operator::Literal) {
        hash_combine(h, static_cast<size_t>(var_id));
    }

    return h;
}

} // namespace Cosy
