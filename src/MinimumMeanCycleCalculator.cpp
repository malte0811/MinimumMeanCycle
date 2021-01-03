#include <iostream>
#include <cassert>
#include <cmath>
#include <set>
#include <map>
#include <numeric>
#include "MinimumMeanCycleCalculator.h"
#include "TJoinCalculator.h"

namespace MMC {

MinimumMeanCycleCalculator::MinimumMeanCycleCalculator(Graph const& graph) : _graph(graph) {}

std::optional<std::vector<EdgeId>> MinimumMeanCycleCalculator::find_mmc() {
    std::vector<EdgeId> result_cycle;
    {
        std::vector<EdgeId> all_edges(_graph.num_edges().get());
        std::iota(all_edges.begin(), all_edges.end(), EdgeId{0});
        auto const start_cycle = find_any_circuit(all_edges);
        if (not start_cycle) {
            return std::nullopt;
        }
        result_cycle = *start_cycle;
    }
    double gamma = get_average_cost(result_cycle);
    double delta_gamma = 0;
    do {
        TJoinCalculator calc(_graph);
        std::cout << "Calculating join with gamma=" << gamma << '\n';
        auto const& min_join = calc.get_minimum_zero_join([gamma](double orig_cost) {
            return orig_cost - gamma;
        });
        if (not min_join.empty()) {
            auto const gamma_next = get_average_cost(min_join);
            delta_gamma = gamma - gamma_next;
            assert(delta_gamma >= 0);
            gamma = gamma_next;
            result_cycle = find_any_circuit(min_join).value();
        } else {
            delta_gamma = 0;
        }
    } while (delta_gamma > 1e-3);
    return result_cycle;
}

std::optional<std::vector<EdgeId>>
MinimumMeanCycleCalculator::find_any_circuit(std::vector<EdgeId> const& edges) const {
    // Find a circuit, heuristically optimize for low mean cost by using a DFS with cheap edges first
    std::set<NodeId> unvisited_nodes;
    std::map<NodeId, std::vector<std::pair<EdgeId, NodeId>>> edge_map;
    for (auto const& edge_id : edges) {
        auto const ends = _graph.edge_ends(edge_id);
        for (size_t i = 0; i < ends.size(); ++i) {
            unvisited_nodes.insert(ends[i]);
            edge_map[ends[i]].emplace_back(edge_id, ends[1 - i]);
        }
    }
    for (auto&[_, edges] : edge_map) {
        std::sort(edges.begin(), edges.end(), [this](auto const& pair_a, auto const& pair_b) {
            return _graph.edge_weight(pair_a.first) < _graph.edge_weight(pair_b.first);
        });
    }
    struct StackElement {
        EdgeId parent_edge;
        NodeId current_node;
        size_t next_edge;
    };
    while (not unvisited_nodes.empty()) {
        auto const root = *unvisited_nodes.begin();
        unvisited_nodes.erase(unvisited_nodes.begin());
        std::vector<StackElement> stack{StackElement{EdgeId{0}, root, 0}};
        std::set<NodeId> in_stack{root};
        while (not stack.empty()) {
            auto& stack_top = stack.back();
            auto const& edges_at_current = edge_map.at(stack_top.current_node);
            if (stack_top.next_edge < edges_at_current.size()) {
                auto const&[edge, next] = edges_at_current.at(stack_top.next_edge);
                ++stack_top.next_edge;
                if (edge == stack_top.parent_edge) {
                    continue;
                }
                if (in_stack.count(next)) {
                    std::vector<EdgeId> circuit{edge};
                    while (stack.back().current_node != next) {
                        circuit.push_back(stack.back().parent_edge);
                        stack.pop_back();
                    }
                    return circuit;
                } else {
                    stack.push_back(StackElement{edge, next, 0});
                    in_stack.insert(next);
                }
            } else {
                in_stack.erase(stack_top.current_node);
                stack.pop_back();
            }
        }
    }
    return std::nullopt;
}

double MinimumMeanCycleCalculator::get_average_cost(std::vector<EdgeId> const& edges) const {
    double total_cost_current_cf = 0;
    for (auto const& join_edge : edges) {
        total_cost_current_cf += _graph.edge_weight(join_edge);
    }
    assert(total_cost_current_cf <= 0);
    return total_cost_current_cf / edges.size();
}

}
