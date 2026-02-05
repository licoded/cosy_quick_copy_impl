#pragma once

namespace Cosy {

class Formula;
class FormulaBuilder;

/**
 * @brief Simplify a formula by applying canonicalization and simplification rules
 *
 * This function applies the following transformations:
 * - Flattening: (a & (b & c)) → {a, b, c}
 * - Commutativity: a & b = b & a
 * - Associativity: (a & b) & c = a & (b & c)
 * - Constant propagation: False & x → False, True | x → True
 * - Identity elimination: True & x → x, False | x → x
 *
 * TODO: Add complementary literal detection (a & !a → False, a | !a → True)
 * TODO: Extend to Until/Release operators
 *
 * @param formula The formula to simplify
 * @param builder The formula builder to use for creating new formulas
 * @return Simplified formula (may be the same as input if no simplification applies)
 */
Formula* simplify(Formula* formula, FormulaBuilder& builder);

} // namespace Cosy
