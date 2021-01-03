#ifndef MINIMUMMEANCYCLE_STEPWISEPATHCALCULATOR_H
#define MINIMUMMEANCYCLE_STEPWISEPATHCALCULATOR_H

#include "graph.hpp"
#include "ShortestPathCalculator.h"
#include "heaps/heap.h"

namespace MMC {

class StepwisePathCalculator {
public:
    StepwisePathCalculator(Graph const& graph, std::function<double(double)> const& cost_transform, NodeId source);

    std::optional<NodeId> step();

    [[nodiscard]] Path get_path_to(NodeId target) const;

private:
    struct NodeData {
        NodeId last;
        double distance;
        size_t heap_index;
        bool fixed;
    };

    Graph const& _graph;
    std::function<double(double)> const& _cost_transform;
    NodeId _source;
    std::vector<NodeData> _node_data;
    heap::Heap<NodeId, double> _heap;
};

}

#endif //MINIMUMMEANCYCLE_STEPWISEPATHCALCULATOR_H
