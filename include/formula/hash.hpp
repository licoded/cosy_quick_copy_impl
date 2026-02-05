#pragma once

#include "operator.hpp"

namespace Cosy {

class Formula;

/**
 * @brief Hash computation for Formula objects
 *
 * This class is responsible for computing hash values for Formula objects.
 * It is a friend of Formula to allow for potential future optimizations
 * that may require access to Formula internals.
 *
 * The hash computation can be easily replaced by modifying the implementation
 * in hash.cpp without affecting the rest of the codebase.
 */
class FormulaHasher {
    friend class Formula;

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
