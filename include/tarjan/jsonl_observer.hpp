#pragma once

#include "tarjan/tarjan_observer.hpp"
#include "nlohmann/json.hpp"

#include <fstream>

namespace tarjan {

// ============================================================================
// JsonlObserver - JSONL 格式追踪日志记录器
// ============================================================================
template <typename Types>
class JsonlObserver : public ITarjanObserver<Types> {
public:
    using Node = typename Types::Node;
    using Edge = typename Types::Edge;

    explicit JsonlObserver(const std::string& filepath)
        : file_(filepath), step_(0), scc_idx_(0) {}

    void onNodeVisited(Node* node) override {
        nlohmann::json j;
        j["step"] = ++step_;
        j["event_type"] = "DISCOVER_NODE";
        j["data"]["id"] = node->getHashId();
        file_ << j.dump() << "\n";
    }

    void onTransition(Node* from, Node* to, const Edge&) override {
        nlohmann::json j;
        j["step"] = ++step_;
        j["event_type"] = "DISCOVER_EDGE";
        j["data"]["src"] = from->getHashId();
        j["data"]["dst"] = to->getHashId();
        file_ << j.dump() << "\n";
    }

    void onSccCompleted(const std::vector<Node*>& scc) override {
        nlohmann::json j;
        j["step"] = ++step_;
        j["event_type"] = "SCC_FOUND";
        j["data"]["scc_idx"] = ++scc_idx_;
        j["data"]["nodes"] = nlohmann::json::array();
        for (auto* node : scc) {
            j["data"]["nodes"].push_back(node->getHashId());
        }
        file_ << j.dump() << "\n";
    }

    void close() { file_.close(); }

private:
    std::ofstream file_;
    int step_;
    int scc_idx_;
};

} // namespace tarjan
