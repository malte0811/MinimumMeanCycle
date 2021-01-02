#include <cassert>
#include <iostream>
#include "TJoinCalculator.h"
#include "ShortestPathCalculator.h"
#include "blossomv/PerfectMatching.h"

namespace MMC {

TJoin MMC::TJoinCalculator::get_minimum_zero_join(std::function<double(double)> const& cost_transform) const {
    std::vector<bool> node_is_odd(_base_graph.num_nodes(), false);
    std::vector<EdgeId> negative_edges;
    for (NodeId lower_end = 0; lower_end < _base_graph.num_nodes(); ++lower_end) {
        auto const& node = _base_graph.node(lower_end);
        for (auto const& half_edge_id : node.outgoing_halfedges()) {
            auto const& edge = _base_graph.halfedge(half_edge_id);
            auto const higher_end = edge.target();
            if (_base_graph.edge_cost(half_edge_id, cost_transform) < 0 and higher_end > lower_end) {
                for (auto end : {lower_end, higher_end}) {
                    node_is_odd[end] = !node_is_odd[end];
                }
                negative_edges.push_back(_base_graph.edge_id(half_edge_id));
            }
        }
    }
    assert(std::is_sorted(negative_edges.begin(), negative_edges.end()));
    std::vector<NodeId> odd_nodes;
    for (NodeId i = 0; i < _base_graph.num_nodes(); ++i) {
        if (node_is_odd.at(i)) {
            odd_nodes.push_back(i);
        }
    }
    auto base_result = get_minimum_cost_t_join(odd_nodes, [&cost_transform](double c) {
        return std::abs(cost_transform(c));
    });
    //TODO less stupid?
    std::sort(base_result.begin(), base_result.end());
    //TODO size estimate?
    TJoin result_join;
    std::set_symmetric_difference(
            negative_edges.begin(), negative_edges.end(),
            base_result.begin(), base_result.end(),
            std::back_inserter(result_join)
    );
    return result_join;
}

TJoin TJoinCalculator::get_minimum_cost_t_join(
        std::vector<NodeId> const& odd_nodes, std::function<double(double)> const& cost_transform
) const {
#ifndef NDEBUG
    for (HalfEdgeId i = 0; i < _base_graph.num_halfedges(); ++i) {
        assert(_base_graph.edge_cost(i, cost_transform) >= 0);
    }
#endif
    ShortestPathCalculator path_calc(_base_graph, cost_transform);
    std::cout << "Starting shortest path calc, num odd: " << odd_nodes.size() << '\n';
    auto const paths = path_calc.some_pairs_shortest_paths(odd_nodes);
    std::cout << "Path calc done\n";
    PerfectMatching solver{
            static_cast<int>(odd_nodes.size()),
            static_cast<int>((odd_nodes.size() * (odd_nodes.size() - 1)) / 2)
    };
    //solver.options.verbose = false;
    for (EdgeId lower = 0; lower < odd_nodes.size(); ++lower) {
        for (EdgeId higher = lower + 1; higher < odd_nodes.size(); ++higher) {
            solver.AddEdge(lower, higher, paths.get_path(lower, higher).path_cost);
        }
    }
    solver.Solve();
    TJoin result;
    for (size_t odd_node_index = 0; odd_node_index < odd_nodes.size(); ++odd_node_index) {
        size_t const matches_to_index = solver.GetMatch(odd_node_index);
        if (matches_to_index < odd_node_index) {
            //TODO deal with duplicate zero-weight edges (not necessary for correctness, but for convergence speed)
            auto const& path = paths.get_path(matches_to_index, odd_node_index);
            std::copy(path.path_edges.begin(), path.path_edges.end(), std::back_inserter(result));
        }
    }
    return result;
}

TJoinCalculator::TJoinCalculator(Graph const& baseGraph) : _base_graph(baseGraph) {}

}