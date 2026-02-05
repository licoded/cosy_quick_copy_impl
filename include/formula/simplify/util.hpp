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
 * @brief Rebuild a binary operator chain from a set of terms
 *
 * Creates a right-leaning chain: {a, b, c} → op(a, op(b, c))
 * - For AND: returns True if empty, single term if only one, otherwise AND chain
 * - For OR: returns False if empty, single term if only one, otherwise OR chain
 *
 * Deterministic ordering from std::set ensures consistent hash consing
 *
 * @param builder FormulaBuilder for creating formulas
 * @param terms Set of terms to chain
 * @param op Operator type (Operator::And or Operator::Or)
 * @return Rebuilt formula
 */
Formula* rebuild_chain(FormulaBuilder& builder, const std::set<Formula*>& terms, Operator op);

// TODO: Add has_complementary_literals for detecting a & !a or a | !a

} // namespace SimplifyUtil

} // namespace Cosy
