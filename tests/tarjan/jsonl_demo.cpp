#include "graph/simple_graph.hpp"
#include "tarjan/jsonl_observer.hpp"

#include <fstream>
#include <iostream>

int main() {
    // 构建图:
    // 0 -> 1 -> 2 -> 0   (SCC1: {0,1,2})
    // 1 -> 3 -> 4 -> 3   (SCC2: {3,4})
    // 4 -> 5             (SingleNode: {5})
    simple_graph::GraphBuilder graph;
    graph.addNodeWithEdges(0, {1});
    graph.addNodeWithEdges(1, {2, 3});
    graph.addNodeWithEdges(2, {0});
    graph.addNodeWithEdges(3, {4});
    graph.addNodeWithEdges(4, {3, 5});
    graph.addNodeWithEdges(5, {});

    // 创建 JSONL 观察者
    tarjan::JsonlObserver<simple_graph::Types> observer("/tmp/tarjan_trace.jsonl");

    // 运行 Tarjan 算法
    tarjan::Tarjan<simple_graph::Types> tarjan(&graph.collector, &graph.factory);
    tarjan.addObserver(&observer);
    tarjan.dfsSearch(&graph.nodes[0]);
    observer.close();

    // 输出结果
    std::cout << "JSONL trace written to /tmp/tarjan_trace.jsonl\n\n";
    std::cout << "Content:\n";

    std::ifstream file("/tmp/tarjan_trace.jsonl");
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << "\n";
    }

    return 0;
}
