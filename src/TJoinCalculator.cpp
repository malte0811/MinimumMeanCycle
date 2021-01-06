#include <cassert>
#include <iostream>
#include <map>
#include "TJoinCalculator.h"
#include "ShortestPathCalculator.h"
#include "blossomv/PerfectMatching.h"

namespace MMC {

TJoinCalculator::TJoinCalculator(Graph const& baseGraph) : _base_graph(baseGraph) {}

TJoin MMC::TJoinCalculator::get_minimum_zero_join(std::function<long(long)> const& cost_transform) const {
    std::vector<bool> node_is_odd(_base_graph.num_nodes(), false);
    std::vector<Edge> negative_edges;
    for (NodeId lower = 0; lower < _base_graph.num_nodes(); ++lower) {
        for (NodeId upper = lower + 1; upper < _base_graph.num_nodes(); ++upper) {
            Edge const edge{lower, upper};
            if (_base_graph.edge_cost(edge, cost_transform) < 0) {
                for (auto const end : {lower, upper}) {
                    node_is_odd[end] = not node_is_odd[end];
                }
                negative_edges.push_back(edge);
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
    auto base_result = get_minimum_cost_t_join_nonnegative_costs(odd_nodes, [&cost_transform](double c) {
        return std::abs(cost_transform(c));
    });
    std::sort(base_result.begin(), base_result.end());
    TJoin result_join;
    std::set_symmetric_difference(
            negative_edges.begin(), negative_edges.end(),
            base_result.begin(), base_result.end(),
            std::back_inserter(result_join)
    );
    return result_join;
}

TJoin TJoinCalculator::get_minimum_cost_t_join_nonnegative_costs(
        std::vector<NodeId> const& odd_nodes, std::function<long(long)> const& cost_transform
) const {
#ifndef NDEBUG
    for (NodeId lower = 0; lower < _base_graph.num_nodes(); ++lower) {
        for (NodeId higher = lower + 1; higher < _base_graph.num_nodes(); ++higher) {
            Edge edge{lower, higher};
            assert(not _base_graph.edge_exists(edge) or _base_graph.edge_cost(edge, cost_transform) >= 0);
        }
    }
#endif
    std::map<std::pair<size_t, size_t>, Path> paths;
    for (size_t lower = 0; lower < odd_nodes.size(); ++lower) {
        ShortestPathCalculator calc(odd_nodes.at(lower), _base_graph, cost_transform);
        calc.run_until_found(odd_nodes.begin() + lower + 1, odd_nodes.end());
        for (size_t higher = lower + 1; higher < odd_nodes.size(); ++higher) {
            if (auto const path = calc.make_path(odd_nodes.at(higher))) {
                paths.insert({std::make_pair(lower, higher), *path});
            }
        }
    }

    PerfectMatching solver{static_cast<int>(odd_nodes.size()), static_cast<int>(paths.size())};
    for (size_t lower = 0; lower < odd_nodes.size(); ++lower) {
        for (size_t higher = lower + 1; higher < odd_nodes.size(); ++higher) {
            solver.AddEdge(lower, higher, paths.at(std::make_pair(lower, higher)).path_cost);
        }
    }
    solver.Solve();
    TJoin result;
    for (size_t odd_node_index = 0; odd_node_index < odd_nodes.size(); ++odd_node_index) {
        size_t const matched_to_index = solver.GetMatch(odd_node_index);
        if (matched_to_index < odd_node_index) {
            auto const& path = paths.at(std::make_pair(matched_to_index, odd_node_index)).path_edges;
            std::copy(path.begin(), path.end(), std::back_inserter(result));
        }
    }
    // Remove pairs of duplicate edges
    std::sort(result.begin(), result.end());
    auto it = result.begin();
    while (it != result.end() and it + 1 != result.end()) {
        if (*it == *(it + 1)) {
            it = result.erase(it, it + 2);
        } else {
            ++it;
        }
    }
    return result;
}

}
