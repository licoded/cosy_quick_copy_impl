#pragma once

#include "operator.hpp"

namespace Cosy {

class Formula;

/**
 * @brief Combine a hash value into a seed using boost::hash_combine style
 *
 * Uses the golden ratio constant 0x9e3779b9 for better hash distribution.
 * This is the same algorithm used by boost::hash_combine.
 *
 * @param seed The seed to combine into (modified in place)
 * @param value The value to combine
 */
inline void hash_combine(size_t& seed, size_t value) {
    seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

/**
 * @brief Hash computation for Formula objects
 *
 * This class is responsible for computing hash values for Formula objects.
 * The hash computation can be easily replaced by modifying the implementation
 * in hash.cpp without affecting the rest of the codebase.
 */
class FormulaHasher {
public:
    /**
     * @brief Compute hash value for a formula
     *
     * Uses boost::hash_combine style algorithm.
     * Hash is based on: operator type, left child, right child, and variable ID.
     *
     * @param op Operator type
     * @param left Left child formula (may be nullptr)
     * @param right Right child formula (may be nullptr)
     * @param var_id Variable ID (only used for Literal operator)
     * @return Computed hash value
     */
    static size_t compute(Operator op, Formula* left, Formula* right, unsigned int var_id);
};

} // namespace Cosy
