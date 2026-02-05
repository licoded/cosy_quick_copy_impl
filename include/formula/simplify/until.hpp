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
 * - True U a → True (eventually a, but "true until a" is always true)
 * - a U True → True
 * - a U a → a
 * - (a U b) U b → a U b (idempotent with same right operand)
 * - a U (a U b) → a U b (idempotent with same left operand)
 * - (a U b) U c → a U (b U c) when b -> c (chain rule)
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