#pragma once

namespace Cosy {

class Formula;
class FormulaBuilder;

/**
 * @brief Simplifier for WNext (X) operator
 *
 * Simplification rules:
 * - X True → True
 */
class WNextSimplifier {
public:
    /**
     * @brief Simplify a WNext formula
     *
     * @param formula The WNext formula to simplify
     * @param builder FormulaBuilder for creating formulas
     * @return Simplified formula
     */
    static Formula* simplify(Formula* formula, FormulaBuilder& builder);
};

} // namespace Cosy
