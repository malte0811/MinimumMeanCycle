#include <cassert>
#include <iostream>
#include "TJoinCalculator.h"
#include "ShortestPathCalculator.h"
#include "blossomv/PerfectMatching.h"

namespace MMC {

TJoin MMC::TJoinCalculator::get_minimum_zero_join(std::function<double(double)> const& cost_transform) const {
    std::vector<bool> node_is_odd(_base_graph.num_nodes().get(), false);
    std::vector<EdgeId> negative_edges;
    for (EdgeId edge_id{0}; edge_id < _base_graph.num_edges(); ++edge_id) {
        if (_base_graph.edge_cost(edge_id, cost_transform) < 0) {
            for (auto const end : _base_graph.edge_ends(edge_id)) {
                node_is_odd[end.get()] = not node_is_odd[end.get()];
            }
            negative_edges.push_back(edge_id);
        }
    }
    assert(std::is_sorted(negative_edges.begin(), negative_edges.end()));
    std::vector<NodeId> odd_nodes;
    for (NodeId i{0}; i < _base_graph.num_nodes(); ++i) {
        if (node_is_odd.at(i.get())) {
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
    for (EdgeId i{0}; i < _base_graph.num_edges(); ++i) {
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
    solver.options.verbose = false;
    for (size_t lower = 0; lower < odd_nodes.size(); ++lower) {
        for (size_t higher = lower + 1; higher < odd_nodes.size(); ++higher) {
            solver.AddEdge(lower, higher, paths.get_path(lower, higher).path_cost);
        }
    }
    solver.Solve();
    TJoin result;
    for (size_t odd_node_index = 0; odd_node_index < odd_nodes.size(); ++odd_node_index) {
        size_t const matches_to_index = solver.GetMatch(odd_node_index);
        if (matches_to_index < odd_node_index) {
            auto const& path = paths.get_path(matches_to_index, odd_node_index);
            std::copy(path.path_edges.begin(), path.path_edges.end(), std::back_inserter(result));
        }
    }
    std::sort(result.begin(), result.end());
    for (auto it = result.begin(); it != result.end() and it + 1 != result.end(); ++it) {
        if (*it == *(it + 1)) {
            result.erase(it, it + 2);
        }
    }
    return result;
}

TJoinCalculator::TJoinCalculator(Graph const& baseGraph) : _base_graph(baseGraph) {}

}