#include "formula/hash.hpp"
#include "formula/formula.hpp"

namespace Cosy {

size_t FormulaHasher::compute(Operator op, Formula* left, Formula* right, unsigned int var_id) {
    size_t h = 0;

    hash_combine(h, static_cast<size_t>(op));

    if (left)  hash_combine(h, left->hash());
    if (right) hash_combine(h, right->hash());

    if (op == Operator::Literal) {
        hash_combine(h, static_cast<size_t>(var_id));
    }

    return h;
}

} // namespace Cosy
