#include <cassert>
#include "ShortestPathCalculator.h"
#include "StepwisePathCalculator.h"

namespace MMC {

ShortestPathCalculator::ShortestPathCalculator(
        MMC::Graph const& graph, std::function<long(long)> const& cost_transform
) : _graph(graph),
    _cost_transform(cost_transform) {}

PathMatrix ShortestPathCalculator::some_pairs_shortest_paths(std::vector<NodeId> const& endpoints) {
    if (endpoints.empty()) {
        return PathMatrix(std::vector<std::vector<Path>>{});
    }
    //TODO disconnected graphs?
    //TODO this is a very naive implementation and needs to be optimized
    std::vector<NodeId> targets = endpoints;
    targets.pop_back();
    std::vector<std::vector<Path>> paths;
    for (size_t source_index = endpoints.size() - 1; source_index > 0; --source_index) {
        paths.push_back(paths_from_root(endpoints.at(source_index), targets));
        targets.pop_back();
    }
    //TODO handle manually? On the other hand it's not at all perf critical
    paths.emplace_back();
    std::reverse(paths.begin(), paths.end());
    assert(targets.empty());
    return PathMatrix(std::move(paths));
}

std::vector<Path> ShortestPathCalculator::paths_from_root(NodeId const& source, std::vector<NodeId> const& targets) {
    size_t num_targets_found = 0;
    StepwisePathCalculator stepwise_calc(_graph, _cost_transform, source);
    while (num_targets_found < targets.size()) {
        auto const& fixed_node = stepwise_calc.step();
        assert(fixed_node.has_value());
        if (std::find(targets.begin(), targets.end(), fixed_node.value()) != targets.end()) {
            ++num_targets_found;
        }
    }

    std::vector<Path> paths;
    paths.reserve(targets.size());
    for (auto const& target : targets) {
        paths.push_back(stepwise_calc.get_path_to(target));
    }
    return paths;
}

Path const& PathMatrix::get_path(size_t lower, size_t higher) const {
    assert(lower < higher);
    return _paths.at(higher).at(lower);
}

PathMatrix::PathMatrix(std::vector<std::vector<Path>>&& paths) : _paths(std::move(paths)) {}

}
