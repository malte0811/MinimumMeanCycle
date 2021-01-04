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

std::optional<std::vector<Edge>> MinimumMeanCycleCalculator::find_mmc() {
    std::vector<Edge> result_cycle;
    {
        std::vector<Edge> all_edges;
        for (size_t lower = 0; lower < _graph.num_nodes(); ++lower) {
            for (size_t higher = lower + 1; higher < _graph.num_nodes(); ++higher) {
                all_edges.emplace_back(lower, higher);
            }
        }
        auto const start_cycle = find_any_circuit(all_edges);
        if (not start_cycle) {
            return std::nullopt;
        }
        result_cycle = *start_cycle;
    }
    auto gamma = get_average_cost(result_cycle);
    auto gamma_last = gamma;
    do {
        TJoinCalculator calc(_graph);
        std::cout << "Calculating join with gamma=" << static_cast<double>(gamma) << '\n';
        auto const& min_join = calc.get_minimum_zero_join([gamma](long orig_cost) {
            return orig_cost * gamma.num_edges - gamma.cost_sum;
        });
        if (not min_join.empty()) {
            auto const gamma_next = get_average_cost(min_join);
            gamma_last = gamma;
            gamma = gamma_next;
            result_cycle = find_any_circuit(min_join).value();
        } else {
            std::cout << "Empty join!\n";
            gamma = {0, 1};
        }
    } while (gamma != gamma_last and gamma.cost_sum != 0);
    return result_cycle;
}

std::optional<std::vector<Edge>>
MinimumMeanCycleCalculator::find_any_circuit(std::vector<Edge> const& edges) const {
    // Find a circuit, heuristically optimize for low mean cost by using a DFS with cheap edges first
    std::set<NodeId> unvisited_nodes;
    std::map<NodeId, std::vector<std::pair<Edge, NodeId>>> edge_map;
    for (auto const& edge_id : edges) {
        std::array<NodeId, 2> ends{edge_id.first, edge_id.second};
        for (size_t i = 0; i < ends.size(); ++i) {
            unvisited_nodes.insert(ends[i]);
            edge_map[ends[i]].emplace_back(edge_id, ends[1 - i]);
        }
    }
    for (auto&[_, edges] : edge_map) {
        std::sort(edges.begin(), edges.end(), [this](auto const& pair_a, auto const& pair_b) {
            return _graph.edge_cost(pair_a.first) < _graph.edge_cost(pair_b.first);
        });
    }
    struct StackElement {
        Edge parent_edge;
        NodeId current_node;
        size_t next_edge;
    };
    while (not unvisited_nodes.empty()) {
        auto const root = *unvisited_nodes.begin();
        unvisited_nodes.erase(unvisited_nodes.begin());
        std::vector<StackElement> stack{StackElement{Edge{0, 0}, root, 0}};
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
                    std::vector<Edge> circuit{edge};
                    long cost = _graph.edge_cost(edge);
                    while (stack.back().current_node != next) {
                        circuit.push_back(stack.back().parent_edge);
                        cost += _graph.edge_cost(stack.back().parent_edge);
                        stack.pop_back();
                    }
                    std::cout << "Avg cost: " << (cost / static_cast<double>(circuit.size())) << '\n';
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

auto MinimumMeanCycleCalculator::get_average_cost(std::vector<Edge> const& edges) const -> Gamma {
    long total_cost = 0;
    for (auto const& join_edge : edges) {
        total_cost += _graph.edge_cost(join_edge);
    }
    assert(total_cost <= 0);
    return Gamma{total_cost, edges.size()};
}

bool MinimumMeanCycleCalculator::Gamma::operator==(MinimumMeanCycleCalculator::Gamma const& other) const {
    return cost_sum * other.num_edges == other.cost_sum * num_edges;
}

bool MinimumMeanCycleCalculator::Gamma::operator!=(MinimumMeanCycleCalculator::Gamma const& other) const {
    return not(*this == other);
}
}
