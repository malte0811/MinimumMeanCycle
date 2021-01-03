#include "StepwisePathCalculator.h"

namespace MMC {
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
    while (current_node != _source) {
        auto const& current_data = _node_data.at(current_node.get());
        edges_reversed.push_back(_graph.edge_id(current_data.half_edge_to_prev));
        current_node = _graph.halfedge(current_data.half_edge_to_prev).target();
    }
    std::reverse(edges_reversed.begin(), edges_reversed.end());
    return {std::move(edges_reversed), _node_data.at(target.get()).distance};
}

}
