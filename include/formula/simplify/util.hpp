#pragma once

#include "formula/operator.hpp"
#include <set>

namespace Cosy {

class Formula;
class FormulaBuilder;

/**
 * @brief Utility functions for formula simplification
 */
namespace SimplifyUtil {

/**
 * @brief Collect binary operator terms into a set (flatten chains)
 *
 * Flattens nested AND/OR structures recursively: `(a & (b & c))` → `{a, b, c}`
 * Uses std::set for deterministic ordering (crucial for hash consing)
 * Collects all terms including True, False (caller checks for them afterward)
 *
 * @param f Formula to collect from
 * @param terms Output set of terms
 * @param op The operator type to flatten (And or Or)
 */
void collect_binary_terms(Formula* f, std::set<Formula*>& terms, Operator op);

/**
 * @brief Check if terms contain complementary literals (a and !a)
 *
 * For AND: detects conflicts (a & !a → False)
 * For OR: detects tautologies (a | !a → True)
 *
 * Algorithm: separate positive/negative literals, check for pairs.
 * Due to hash consing, Not(a) has a unique pointer for identical negations.
 *
 * @param terms Set of terms to check
 * @param builder FormulaBuilder for creating Not formulas
 * @return true if complementary pair found
 */
bool has_complementary_literals(const std::set<Formula*>& terms, FormulaBuilder& builder);

} // namespace SimplifyUtil

} // namespace Cosy
