#ifndef MINIMUMMEANCYCLE_SHORTESTPATHCALCULATOR_H
#define MINIMUMMEANCYCLE_SHORTESTPATHCALCULATOR_H

#include <vector>
#include <optional>
#include <queue>
#include "graph.h"
#include "MinimumMeanCycleCalculator.h"

namespace MMC {

struct Path {
    /// Edges in the path always have the lower vertex ID first. The order of the edges in the vector is unspecified.
    std::vector<Edge> edge_set;
    AccumulatedEdgeWeight path_cost;
};

/**
 * An implementation of Dijkstra's algorithm. It uses a std::priority_queue as the heap. Since this structure does not
 * have an equivalent of decrease_key, nodes are (potentially) added to the heap multiple times. The first extracted
 * occurrence of a node is the one with the shortest distance, all further occurrences will be ignored.
 */
class ShortestPathCalculator {
public:
    /**
     * Initialize the path calculator with the given source and graph. The costs used are abs(cost_transform.apply(-)).
     */
    ShortestPathCalculator(NodeId source, Graph const& graph, Gamma cost_transform);

    /**
     * Run Dijkstra's algorithm until all nodes in the given iterator range have been found or all reachable nodes have
     * been marked
     */
    template<class Iterator>
    void run_until_found(Iterator const& targets_begin, Iterator const& targets_end);

    /**
     * Create a path object representing the shortest path found by Dijkstra's algorithm. Returns an empty optional if
     * no path to this node was found by previous calls to run_until_found. If this target node was in the range passed
     * to run_until_found this implies that source and target are in different connected components.
     */
    [[nodiscard]] std::optional<Path> make_path(NodeId target) const;

private:
    struct NodeData {
        /// Previous node on the shortest path
        NodeId last;
        /// Distance from the source on the shortest path
        AccumulatedEdgeWeight distance;
        /// Is the current distance known to be optimal?
        bool fixed;
    };

    struct HeapEntry {
        NodeId node;
        AccumulatedEdgeWeight distance;

        bool operator>(HeapEntry const& other) const {
            return distance > other.distance;
        }
    };

    /**
     * Runs one iteration of Dijkstra's algorithm. Returns the node whose distance was fixes if one was fixed. If no
     * more unprocessed nodes are found in the heap an empty optional is returned.
     */
    std::optional<NodeId> fix_next_node();

    /// Extract nodes from the heap until an unfixed node is found, returns std::nullopt if none is found
    std::optional<NodeId> extract_next_unfixed_node();

    Graph const& _graph;
    Gamma const _cost_transform;
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
