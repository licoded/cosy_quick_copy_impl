#pragma once

namespace Cosy {

class Formula;
class FormulaBuilder;

/**
 * @brief Unified formula simplifier that coordinates operator-specific simplifiers
 *
 * This class provides a unified interface for simplifying formulas by delegating
 * to operator-specific simplifiers (AndSimplifier, OrSimplifier, etc.)
 *
 * Simplification is applied recursively, simplifying children first before
 * applying operator-specific rules.
 */
class FormulaSimplifier {
public:
    /**
     * @brief Simplify a formula recursively
     *
     * @param formula The formula to simplify
     * @param builder The formula builder for creating new formulas
     * @return Simplified formula (may be the same as input if no simplification applies)
     */
    static Formula* simplify(Formula* formula, FormulaBuilder& builder);
};

} // namespace Cosy
