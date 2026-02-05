#pragma once

#include "formula/operator.hpp"

namespace Cosy {

class Formula;
class FormulaBuilder;

/**
 * @brief Simplifier for Until (U) operator
 *
 * Simplification rules:
 * - False U a → a
 * - a U False → False
 * - True U a → True
 * - a U True → True
 * - a U a → a
 * - a U (a | b) → a | b (right absorption)
 * - a U (a U b) → a U b
 * - a U (b U a) → b U a
 * - (X[!] a) U a → a | (X[!] a)
 * - (X[!] a) U (X[!] b) → X[!](a U b)
 */
class UntilSimplifier {
public:
    /**
     * @brief Simplify an Until formula
     *
     * @param formula The Until formula to simplify
     * @param builder FormulaBuilder for creating formulas
     * @return Simplified formula
     */
    static Formula* simplify(Formula* formula, FormulaBuilder& builder);
};

} // namespace Cosy