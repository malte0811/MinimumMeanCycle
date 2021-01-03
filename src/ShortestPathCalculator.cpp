#include <cassert>
#include <ctime>
#include <chrono>
#include "ShortestPathCalculator.h"
#include "heaps/heap.h"

namespace MMC {

ShortestPathCalculator::ShortestPathCalculator(
        MMC::Graph const& graph, std::function<double(double)> const& cost_transform
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

StepwisePathCalculator::StepwisePathCalculator(
        Graph const& graph, std::function<double(double)> const& cost_transform, NodeId const source
) :
        _graph(graph),
        _cost_transform(cost_transform),
        _source(source),
        _node_data(_graph.num_nodes().get()),
        _heap(
                [this](NodeId const& id) -> size_t& { return _node_data.at(id.get()).heap_index; },
                _graph.num_nodes().get()
        ) {
    NodeData empty_node_data{
            _graph.num_halfedges(),
            std::numeric_limits<double>::max(),
            _graph.num_nodes().get(),
    };
    std::fill(_node_data.begin(), _node_data.end(), empty_node_data);
    _node_data.at(source.get()).distance = 0;
    _heap.insert(source, 0);
}

std::optional<NodeId> StepwisePathCalculator::step() {
    if (_heap.empty()) {
        return std::nullopt;
    }
    auto const& next_id_to_fix = _heap.extract_min();
    auto& data_to_fix = _node_data.at(next_id_to_fix.get());
    for (auto const& half_edge_id : _graph.node(next_id_to_fix).outgoing_halfedges()) {
        auto const& half_edge = _graph.halfedge(half_edge_id);
        auto const& end_node = half_edge.target();
        auto& end_data = _node_data.at(end_node.get());
        auto const& edge_weight = _graph.halfedge_cost(half_edge_id, _cost_transform);
        auto const& distance_via_node = data_to_fix.distance + edge_weight;
        if (end_data.distance > distance_via_node) {
            end_data.distance = distance_via_node;
            end_data.half_edge_to_prev = half_edge.inverse();
            _heap.decrease_or_insert(end_node, distance_via_node);
        }
    }
    return next_id_to_fix;
}

Path StepwisePathCalculator::get_path_to(NodeId const target) const {
    std::vector<EdgeId> edges_reversed;
    auto current_node = target;
    double cost_sum = 0;
    while (current_node != _source) {
        auto const& current_data = _node_data.at(current_node.get());
        edges_reversed.push_back(_graph.edge_id(current_data.half_edge_to_prev));
        cost_sum += _graph.edge_cost(edges_reversed.back(), _cost_transform);
        current_node = _graph.halfedge(current_data.half_edge_to_prev).target();
    }
    std::reverse(edges_reversed.begin(), edges_reversed.end());
    assert(_node_data.at(target.get()).distance == cost_sum);
    return {std::move(edges_reversed), _node_data.at(target.get()).distance};
}

}
