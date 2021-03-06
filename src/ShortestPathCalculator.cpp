#include "ShortestPathCalculator.h"
#include <cassert>
#include <iostream>

namespace MMC {

ShortestPathCalculator::ShortestPathCalculator(
        NodeId const source, MMC::Graph const& graph, Gamma cost_transform
) : _graph(graph),
    _cost_transform(cost_transform),
    _source(source),
    _node_data(_graph.num_nodes()) {
    NodeData empty_node_data{0, std::numeric_limits<AccumulatedEdgeWeight>::max(), false};
    std::fill(_node_data.begin(), _node_data.end(), empty_node_data);
    _node_data.at(source).distance = 0;
    _heap.push(HeapEntry{source, 0});
}

std::optional<NodeId> ShortestPathCalculator::fix_next_node() {
    auto const next_id_to_fix_opt = extract_next_unfixed_node();
    if (not next_id_to_fix_opt) {
        return std::nullopt;
    }
    auto const next_id_to_fix = next_id_to_fix_opt.value();
    _node_data.at(next_id_to_fix).fixed = true;

    auto const distance_to_fixed = _node_data.at(next_id_to_fix).distance;
    for (NodeId other_end = 0; other_end < _graph.num_nodes(); ++other_end) {
        auto& end_data = _node_data.at(other_end);
        if (end_data.fixed) {
            continue;
        }
        // Order of the vertices in the edge is logically irrelevant, but highly important for performance due to CPU
        // caches
        Edge const edge{next_id_to_fix, other_end,};
        if (not _graph.edge_exists(edge)) {
            continue;
        }
        auto const edge_weight = std::abs(_cost_transform.apply(_graph.edge_cost(edge)));
        auto const distance_via_node = distance_to_fixed + edge_weight;
        if (end_data.distance > distance_via_node) {
            end_data.distance = distance_via_node;
            end_data.last = next_id_to_fix;
            _heap.push(HeapEntry{other_end, distance_via_node});
        }
    }
    return next_id_to_fix;
}

std::optional<Path> ShortestPathCalculator::make_path(NodeId const target) const {
    if (not _node_data.at(target).fixed) {
        return std::nullopt;
    }
    std::vector<Edge> edges;
    auto current_node = target;
    while (current_node != _source) {
        auto const& current_data = _node_data.at(current_node);
        auto const&[lower, higher] = std::minmax(current_data.last, current_node);
        edges.emplace_back(lower, higher);
        current_node = current_data.last;
    }
    return Path{std::move(edges), _node_data.at(target).distance};
}

std::optional<NodeId> ShortestPathCalculator::extract_next_unfixed_node() {
    while (not _heap.empty()) {
        auto const to_fix = _heap.top().node;
        _heap.pop();
        if (not _node_data.at(to_fix).fixed) {
            return to_fix;
        }
    }
    return std::nullopt;
}

}
