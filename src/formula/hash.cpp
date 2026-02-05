#include "formula/hash.hpp"
#include "formula/formula.hpp"

namespace Cosy {

size_t FormulaHasher::compute(Operator op, Formula* left, Formula* right, unsigned int var_id) {
    // Hash combination using boost::hash_combine style
    size_t h = static_cast<size_t>(op);

    if (left) {
        h ^= left->hash() + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
    if (right) {
        h ^= right->hash() + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
    if (op == Operator::Literal) {
        h ^= static_cast<size_t>(var_id) + 0x9e3779b9 + (h << 6) + (h >> 2);
    }

    return h;
}

} // namespace Cosy
