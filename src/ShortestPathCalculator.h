#ifndef MINIMUMMEANCYCLE_SHORTESTPATHCALCULATOR_H
#define MINIMUMMEANCYCLE_SHORTESTPATHCALCULATOR_H

#include <vector>
#include <optional>
#include "graph.h"
#include "heaps/heap.h"

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
        size_t heap_index;
        bool fixed;
    };

    NodeId fix_next_node();

    Graph const& _graph;
    std::function<long(long)> const& _cost_transform;
    NodeId const _source;
    std::vector<NodeData> _node_data;
    heap::Heap<NodeId, long> _heap;
};

template<class Iterator>
inline void ShortestPathCalculator::run_until_found(Iterator const& targets_begin, Iterator const& targets_end) {
    auto const num_targets = static_cast<size_t>(std::distance(targets_begin, targets_end));
    size_t num_targets_found = 0;
    while (num_targets_found < num_targets and not _heap.empty()) {
        auto const& fixed_node = fix_next_node();
        if (std::find(targets_begin, targets_end, fixed_node) != targets_end) {
            ++num_targets_found;
        }
    }
}

}

#endif //MINIMUMMEANCYCLE_SHORTESTPATHCALCULATOR_H
