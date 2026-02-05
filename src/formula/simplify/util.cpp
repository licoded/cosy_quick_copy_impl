#include "formula/simplify/util.hpp"
#include "formula/builder.hpp"
#include "formula/formula.hpp"

namespace Cosy {
namespace SimplifyUtil {

void collect_binary_terms(Formula* f, std::set<Formula*>& terms, Operator op) {
    if (!f) return;

    if (f->op() == op) {
        // Flatten chain: collect both sides
        collect_binary_terms(f->left(), terms, op);
        collect_binary_terms(f->right(), terms, op);
    } else {
        // Collect all terms (including True, False)
        terms.insert(f);
    }
}

Formula* rebuild_chain(FormulaBuilder& builder, const std::set<Formula*>& terms, Operator op) {
    if (terms.empty()) {
        // Identity element
        return (op == Operator::And) ? builder.make_true() : builder.make_false();
    }

    if (terms.size() == 1) {
        return *terms.begin();
    }

    // Build right-leaning chain: a & b & c → &(a, &(b, c))
    auto it = terms.begin();
    Formula* result = *it;
    ++it;

    for (; it != terms.end(); ++it) {
        result = builder.make_binary(op, result, *it);
    }

    return result;
}

} // namespace SimplifyUtil
} // namespace Cosy
