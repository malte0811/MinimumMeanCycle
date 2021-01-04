#ifndef MINIMUMMEANCYCLE_SHORTESTPATHCALCULATOR_H
#define MINIMUMMEANCYCLE_SHORTESTPATHCALCULATOR_H

#include <vector>
#include <optional>
#include "graph.hpp"

namespace MMC {

struct Path {
    std::vector<Edge> path_edges;
    long path_cost;
};

class PathMatrix {
public:
    explicit PathMatrix(std::vector<std::vector<Path>>&& paths);

    // Parameters are vector indices in the node set used to create the path matrix
    [[nodiscard]] Path const& get_path(size_t lower, size_t higher) const;

private:
    // First index is strictly greater than second index
    // TODO index shift
    std::vector<std::vector<Path>> _paths;
};

class ShortestPathCalculator {
public:
    ShortestPathCalculator(Graph const& graph, std::function<long(long)> const& cost_transform);

    PathMatrix some_pairs_shortest_paths(std::vector<NodeId> const& endpoints);

    std::vector<Path> paths_from_root(NodeId const& source, std::vector<NodeId> const& targets);

private:
    Graph const& _graph;
    std::function<long(long)> const& _cost_transform;
};

}

#endif //MINIMUMMEANCYCLE_SHORTESTPATHCALCULATOR_H
