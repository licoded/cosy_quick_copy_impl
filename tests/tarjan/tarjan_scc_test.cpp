#include "tarjan/tarjan.hpp"

#include <cassert>
#include <iostream>
#include <set>
#include <map>

// ============================================================================
// 测试图结构
// ============================================================================

struct TestEdge {
    int to;
};

struct TestNode {
    int id;
    std::vector<TestEdge> edges;

    uint64_t getHashId() const { return static_cast<uint64_t>(id); }
};

using Types = tarjan::TarjanTypes<TestNode, TestEdge>;

// ============================================================================
// EdgeIterator 实现
// ============================================================================
class TestEdgeIterator : public tarjan::IEdgeIterator<Types> {
private:
    TestNode* node_;
    size_t index_;

public:
    explicit TestEdgeIterator(TestNode* node) : node_(node), index_(0) {}

    bool hasNext() override { return index_ < node_->edges.size(); }

    TestEdge next() override { return node_->edges[index_++]; }

    TestNode* follow(const TestEdge& edge) override {
        // 这里简化处理，实际应该从图管理器获取目标节点
        // 我们在测试中通过全局 map 获取
        return nodeMap[edge.to];
    }

    static std::map<int, TestNode*> nodeMap;
};

std::map<int, TestNode*> TestEdgeIterator::nodeMap;

// ============================================================================
// EdgeIteratorFactory 实现
// ============================================================================
class TestEdgeIteratorFactory : public tarjan::IEdgeIteratorFactory<Types> {
public:
    std::unique_ptr<tarjan::IEdgeIterator<Types>> createIterator(TestNode* node) override {
        return std::make_unique<TestEdgeIterator>(node);
    }
};

// ============================================================================
// Strategy 实现 - 收集所有 SCC
// ============================================================================
class TestStrategy : public tarjan::ITarjanStrategy<Types> {
public:
    std::vector<std::vector<int>> sccs;

    void onSccFound(std::vector<TestNode*>& scc) override {
        std::vector<int> sccIds;
        for (auto* node : scc) {
            sccIds.push_back(node->id);
        }
        std::sort(sccIds.begin(), sccIds.end());
        sccs.push_back(sccIds);
    }

    bool shouldStop(TestNode* node) override { return false; }
};

// ============================================================================
// Observer 实现 - 打印事件
// ============================================================================
class TestObserver : public tarjan::ITarjanObserver<Types> {
public:
    void onNodeVisited(TestNode* node) override {
        std::cout << "  [Visit] Node " << node->id << std::endl;
    }

    void onTransition(TestNode* from, TestNode* to, const TestEdge& edge) override {
        std::cout << "  [Edge] " << from->id << " -> " << to->id << std::endl;
    }

    void onSccCompleted(const std::vector<TestNode*>& scc) override {
        std::cout << "  [SCC] Found: ";
        for (auto* node : scc) {
            std::cout << node->id << " ";
        }
        std::cout << std::endl;
    }

    void onNodeCompleted(TestNode* node) override {
        std::cout << "  [Done] Node " << node->id << std::endl;
    }
};

// ============================================================================
// 测试用例
// ============================================================================

void testSimpleSCC() {
    std::cout << "=== Test: Simple SCC (1->2->3->1) ===" << std::endl;

    // 构建图: 1 -> 2 -> 3 -> 1 (一个 SCC)
    std::vector<TestNode> nodes = {
        {1, {{2}}},
        {2, {{3}}},
        {3, {{1}}}
    };

    // 注册节点
    TestEdgeIterator::nodeMap.clear();
    for (auto& node : nodes) {
        TestEdgeIterator::nodeMap[node.id] = &node;
    }

    TestStrategy strategy;
    TestEdgeIteratorFactory factory;
    TestObserver observer;

    tarjan::Tarjan<Types> tarjan(&strategy, &factory);
    tarjan.addObserver(&observer);

    tarjan.dfsSearch(&nodes[0]);

    // 验证: 应该找到一个 SCC: {1, 2, 3}
    std::cout << "\nResult: Found " << strategy.sccs.size() << " SCC(s)" << std::endl;
    for (const auto& scc : strategy.sccs) {
        std::cout << "  SCC: { ";
        for (int id : scc) std::cout << id << " ";
        std::cout << "}" << std::endl;
    }

    assert(strategy.sccs.size() == 1);
    std::set<int> expected = {1, 2, 3};
    std::set<int> actual(strategy.sccs[0].begin(), strategy.sccs[0].end());
    assert(actual == expected);

    std::cout << "PASSED\n" << std::endl;
}

void testMultipleSCCs() {
    std::cout << "=== Test: Multiple SCCs ===" << std::endl;

    // 构建图:
    // 0 -> 1 -> 2 -> 0   (SCC1: {0,1,2})
    // 1 -> 3 -> 4 -> 3   (SCC2: {3,4})
    // 4 -> 5             (SingleNode: {5})
    std::vector<TestNode> nodes = {
        {0, {{1}}},
        {1, {{2}, {3}}},
        {2, {{0}}},
        {3, {{4}}},
        {4, {{3}, {5}}},
        {5, {}}
    };

    TestEdgeIterator::nodeMap.clear();
    for (auto& node : nodes) {
        TestEdgeIterator::nodeMap[node.id] = &node;
    }

    TestStrategy strategy;
    TestEdgeIteratorFactory factory;

    tarjan::Tarjan<Types> tarjan(&strategy, &factory);
    tarjan.dfsSearch(&nodes[0]);

    std::cout << "\nResult: Found " << strategy.sccs.size() << " SCC(s)" << std::endl;
    for (const auto& scc : strategy.sccs) {
        std::cout << "  SCC: { ";
        for (int id : scc) std::cout << id << " ";
        std::cout << "}" << std::endl;
    }

    // 验证: 应该找到 3 个 SCC: {0,1,2}, {3,4}, {5}
    // 注: 单节点 5 也是 SCC (low == dfn)
    assert(strategy.sccs.size() == 3);

    // 排序以便比较
    std::sort(strategy.sccs.begin(), strategy.sccs.end());

    std::set<int> scc1(strategy.sccs[0].begin(), strategy.sccs[0].end());
    std::set<int> scc2(strategy.sccs[1].begin(), strategy.sccs[1].end());
    std::set<int> scc3(strategy.sccs[2].begin(), strategy.sccs[2].end());

    assert((scc1 == std::set<int>{0, 1, 2} || scc1 == std::set<int>{3, 4} || scc1 == std::set<int>{5}));
    assert((scc2 == std::set<int>{0, 1, 2} || scc2 == std::set<int>{3, 4} || scc2 == std::set<int>{5}));
    assert((scc3 == std::set<int>{0, 1, 2} || scc3 == std::set<int>{3, 4} || scc3 == std::set<int>{5}));
    assert(scc1 != scc2 && scc2 != scc3 && scc1 != scc3);  // 互不相同

    std::cout << "PASSED\n" << std::endl;
}

void testNoSCC() {
    std::cout << "=== Test: No SCC (DAG) ===" << std::endl;

    // 构建图: 1 -> 2 -> 3 -> 4 (无环)
    std::vector<TestNode> nodes = {
        {1, {{2}}},
        {2, {{3}}},
        {3, {{4}}},
        {4, {}}
    };

    TestEdgeIterator::nodeMap.clear();
    for (auto& node : nodes) {
        TestEdgeIterator::nodeMap[node.id] = &node;
    }

    TestStrategy strategy;
    TestEdgeIteratorFactory factory;

    tarjan::Tarjan<Types> tarjan(&strategy, &factory);
    tarjan.dfsSearch(&nodes[0]);

    std::cout << "\nResult: Found " << strategy.sccs.size() << " SCC(s)" << std::endl;

    // 每个节点自成一个 SCC (自环不算)
    assert(strategy.sccs.size() == 4);

    std::cout << "PASSED\n" << std::endl;
}

void testSelfLoop() {
    std::cout << "=== Test: Self Loop ===" << std::endl;

    // 构建图: 1 -> 1 (自环)
    std::vector<TestNode> nodes = {
        {1, {{1}}}
    };

    TestEdgeIterator::nodeMap.clear();
    for (auto& node : nodes) {
        TestEdgeIterator::nodeMap[node.id] = &node;
    }

    TestStrategy strategy;
    TestEdgeIteratorFactory factory;

    tarjan::Tarjan<Types> tarjan(&strategy, &factory);
    tarjan.dfsSearch(&nodes[0]);

    std::cout << "\nResult: Found " << strategy.sccs.size() << " SCC(s)" << std::endl;

    assert(strategy.sccs.size() == 1);
    assert(strategy.sccs[0].size() == 1);
    assert(strategy.sccs[0][0] == 1);

    std::cout << "PASSED\n" << std::endl;
}

void testMultipleObservers() {
    std::cout << "=== Test: Multiple Observers ===" << std::endl;

    std::vector<TestNode> nodes = {
        {1, {{2}}},
        {2, {{1}}}
    };

    TestEdgeIterator::nodeMap.clear();
    for (auto& node : nodes) {
        TestEdgeIterator::nodeMap[node.id] = &node;
    }

    TestStrategy strategy;
    TestEdgeIteratorFactory factory;

    // 计数观察者
    class CountObserver : public tarjan::ITarjanObserver<Types> {
    public:
        int visitCount = 0;
        int sccCount = 0;

        void onNodeVisited(TestNode*) override { visitCount++; }
        void onSccCompleted(const std::vector<TestNode*>&) override { sccCount++; }
    };

    CountObserver obs1, obs2;

    tarjan::Tarjan<Types> tarjan(&strategy, &factory);
    tarjan.addObserver(&obs1);
    tarjan.addObserver(&obs2);

    tarjan.dfsSearch(&nodes[0]);

    // 两个观察者都应该收到相同的事件
    assert(obs1.visitCount == obs2.visitCount);
    assert(obs1.sccCount == obs2.sccCount);
    assert(obs1.sccCount == 1);

    std::cout << "PASSED\n" << std::endl;
}

int main() {
    std::cout << "Tarjan SCC Test Suite\n" << std::endl;

    testSimpleSCC();
    testMultipleSCCs();
    testNoSCC();
    testSelfLoop();
    testMultipleObservers();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
