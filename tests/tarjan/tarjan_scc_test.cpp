#include "graph/simple_graph.hpp"

#include <catch2/catch_test_macros.hpp>
#include <set>

// ============================================================================
// Tarjan SCC 测试用例
// ============================================================================

TEST_CASE("Simple SCC: 1->2->3->1 forms one SCC", "[tarjan]") {
    simple_graph::GraphBuilder graph;
    graph.addNodeWithEdges(1, {2});
    graph.addNodeWithEdges(2, {3});
    graph.addNodeWithEdges(3, {1});

    tarjan::Tarjan<simple_graph::Types> tarjan(&graph.collector, &graph.factory);
    tarjan.dfsSearch(&graph.nodes[0]);

    REQUIRE(graph.collector.sccs.size() == 1);
    std::set<int> expected = {1, 2, 3};
    std::set<int> actual(graph.collector.sccs[0].begin(), graph.collector.sccs[0].end());
    REQUIRE(actual == expected);
}

TEST_CASE("Multiple SCCs in complex graph", "[tarjan]") {
    simple_graph::GraphBuilder graph;
    // 0 -> 1 -> 2 -> 0   (SCC1: {0,1,2})
    // 1 -> 3 -> 4 -> 3   (SCC2: {3,4})
    // 4 -> 5             (SingleNode: {5})
    graph.addNodeWithEdges(0, {1});
    graph.addNodeWithEdges(1, {2, 3});
    graph.addNodeWithEdges(2, {0});
    graph.addNodeWithEdges(3, {4});
    graph.addNodeWithEdges(4, {3, 5});
    graph.addNodeWithEdges(5, {});

    tarjan::Tarjan<simple_graph::Types> tarjan(&graph.collector, &graph.factory);
    tarjan.dfsSearch(&graph.nodes[0]);

    REQUIRE(graph.collector.sccs.size() == 3);

    auto sccSets = graph.getSccsAsSets();
    REQUIRE(sccSets.count(std::set<int>{0, 1, 2}));
    REQUIRE(sccSets.count(std::set<int>{3, 4}));
    REQUIRE(sccSets.count(std::set<int>{5}));
}

TEST_CASE("DAG has each node as separate SCC", "[tarjan]") {
    simple_graph::GraphBuilder graph;
    // 1 -> 2 -> 3 -> 4 (无环)
    graph.addNodeWithEdges(1, {2});
    graph.addNodeWithEdges(2, {3});
    graph.addNodeWithEdges(3, {4});
    graph.addNodeWithEdges(4, {});

    tarjan::Tarjan<simple_graph::Types> tarjan(&graph.collector, &graph.factory);
    tarjan.dfsSearch(&graph.nodes[0]);

    REQUIRE(graph.collector.sccs.size() == 4);
}

TEST_CASE("Self loop forms single-node SCC", "[tarjan]") {
    simple_graph::GraphBuilder graph;
    // 1 -> 1 (自环)
    graph.addNodeWithEdges(1, {1});

    tarjan::Tarjan<simple_graph::Types> tarjan(&graph.collector, &graph.factory);
    tarjan.dfsSearch(&graph.nodes[0]);

    REQUIRE(graph.collector.sccs.size() == 1);
    REQUIRE(graph.collector.sccs[0].size() == 1);
    REQUIRE(graph.collector.sccs[0][0] == 1);
}

TEST_CASE("Multiple observers receive same events", "[tarjan]") {
    simple_graph::GraphBuilder graph;
    graph.addNodeWithEdges(1, {2});
    graph.addNodeWithEdges(2, {1});

    class CountObserver : public tarjan::ITarjanObserver<simple_graph::Types> {
    public:
        int visitCount = 0;
        int sccCount = 0;

        void onNodeVisited(simple_graph::Node*) override { visitCount++; }
        void onSccCompleted(const std::vector<simple_graph::Node*>&) override { sccCount++; }
    };

    CountObserver obs1, obs2;

    tarjan::Tarjan<simple_graph::Types> tarjan(&graph.collector, &graph.factory);
    tarjan.addObserver(&obs1);
    tarjan.addObserver(&obs2);
    tarjan.dfsSearch(&graph.nodes[0]);

    REQUIRE(obs1.visitCount == obs2.visitCount);
    REQUIRE(obs1.sccCount == obs2.sccCount);
    REQUIRE(obs1.sccCount == 1);
}
