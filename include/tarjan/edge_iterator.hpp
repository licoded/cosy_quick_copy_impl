#pragma once

#include "tarjan_types.hpp"

#include <memory>

namespace tarjan {

// ============================================================================
// IEdgeIterator - 边迭代器接口
// ============================================================================
template <typename Types>
class IEdgeIterator {
public:
    using Node = typename Types::Node;
    using Edge = typename Types::Edge;

    virtual ~IEdgeIterator() = default;

    virtual bool hasNext() = 0;
    virtual Edge next() = 0;
    virtual Node* follow(const Edge& edge) = 0;
};

// ============================================================================
// IEdgeIteratorFactory - 边迭代器工厂接口
// ============================================================================
template <typename Types>
class IEdgeIteratorFactory {
public:
    using Node = typename Types::Node;

    virtual ~IEdgeIteratorFactory() = default;
    virtual std::unique_ptr<IEdgeIterator<Types>> createIterator(Node* node) = 0;
};

} // namespace tarjan
