#ifndef MINIMUMMEANCYCLE_SHORTESTPATHCALCULATOR_H
#define MINIMUMMEANCYCLE_SHORTESTPATHCALCULATOR_H

#include <vector>
#include <optional>
#include <queue>
#include "graph.h"

namespace MMC {

struct Path {
    std::vector<Edge> path_edges;
    long path_cost;
};

class ShortestPathCalculator {
public:
    ShortestPathCalculator(NodeId source, Graph const& graph, std::function<long(long)> const& cost_transform);

    template<class Iterator>
    void run_until_found(Iterator const& targets_begin, Iterator const& targets_end);

    [[nodiscard]] std::optional<Path> make_path(NodeId target) const;

private:
    struct NodeData {
        NodeId last;
        long distance;
        bool fixed;
    };

    struct HeapEntry {
        NodeId node;
        long distance;

        bool operator>(HeapEntry const& other) const {
            return distance > other.distance;
        }
    };

    std::optional<NodeId> fix_next_node();

    Graph const& _graph;
    std::function<long(long)> const& _cost_transform;
    NodeId const _source;
    std::vector<NodeData> _node_data;
    std::priority_queue<HeapEntry, std::vector<HeapEntry>, std::greater<>> _heap;
};

template<class Iterator>
inline void ShortestPathCalculator::run_until_found(Iterator const& targets_begin, Iterator const& targets_end) {
    auto const num_targets = static_cast<size_t>(std::distance(targets_begin, targets_end));
    size_t num_targets_found = 0;
    while (auto const& fixed_node = fix_next_node()) {
        if (std::find(targets_begin, targets_end, *fixed_node) != targets_end) {
            ++num_targets_found;
            if (num_targets_found == num_targets) {
                break;
            }
        }
    }
}

}

#endif //MINIMUMMEANCYCLE_SHORTESTPATHCALCULATOR_H
