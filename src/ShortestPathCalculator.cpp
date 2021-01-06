#include "ShortestPathCalculator.h"
#include <cassert>

namespace MMC {

ShortestPathCalculator::ShortestPathCalculator(
        NodeId const source, MMC::Graph const& graph, std::function<long(long)> const& cost_transform
) : _graph(graph),
    _cost_transform(cost_transform),
    _source(source),
    _node_data(_graph.num_nodes()),
    _heap(
            [this](NodeId const& id) -> size_t& { return _node_data.at(id).heap_index; },
            _graph.num_nodes()
    ) {
    NodeData empty_node_data{0, std::numeric_limits<long>::max(), _graph.num_nodes(), false};
    std::fill(_node_data.begin(), _node_data.end(), empty_node_data);
    _node_data.at(source).distance = 0;
    _heap.insert(source, 0);
}

NodeId ShortestPathCalculator::fix_next_node() {
    assert(not _heap.empty());
    auto const next_id_to_fix = _heap.extract_min();
    _node_data.at(next_id_to_fix).fixed = true;
    auto const distance_to_fixed = _node_data.at(next_id_to_fix).distance;
    for (NodeId other_end = 0; other_end < _graph.num_nodes(); ++other_end) {
        auto& end_data = _node_data.at(other_end);
        if (end_data.fixed) {
            continue;
        }
        // Order of the vertices is irrelevant for correctness, but highly important for performance due to CPU caches
        Edge const edge{next_id_to_fix, other_end,};
        if (not _graph.edge_exists(edge)) {
            continue;
        }
        auto const edge_weight = _graph.edge_cost(edge, _cost_transform);
        auto const distance_via_node = distance_to_fixed + edge_weight;
        if (end_data.distance > distance_via_node) {
            end_data.distance = distance_via_node;
            end_data.last = next_id_to_fix;
            _heap.decrease_or_insert(other_end, distance_via_node);
        }
    }
    return next_id_to_fix;
}

std::optional<Path> ShortestPathCalculator::make_path(NodeId const target) const {
    if (not _node_data.at(target).fixed) {
        return std::nullopt;
    }
    std::vector<Edge> edges_reversed;
    auto current_node = target;
    while (current_node != _source) {
        auto const& current_data = _node_data.at(current_node);
        auto const&[lower, higher] = std::minmax(current_data.last, current_node);
        edges_reversed.emplace_back(lower, higher);
        current_node = current_data.last;
    }
    std::reverse(edges_reversed.begin(), edges_reversed.end());
    return Path{std::move(edges_reversed), _node_data.at(target).distance};
}

}
