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
        // The only condition the proof places on the initial gamma is that it is an upper bound for the mean cost of an
        // MMC. So the mean weight of an arbitrary cycle (in this case chosen with heuristically high mean weight) is a
        // valid choice.
        std::vector<Edge> all_edges;
        for (size_t lower = 0; lower < _graph.num_nodes(); ++lower) {
            for (size_t higher = lower + 1; higher < _graph.num_nodes(); ++higher) {
                Edge const edge{lower, higher};
                if (_graph.edge_exists(edge)) {
                    all_edges.push_back(edge);
                }
            }
        }
        auto const start_cycle = find_heuristically_good_circuit(all_edges);
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
            result_cycle = find_heuristically_good_circuit(min_join).value();
        } else {
            gamma = {0, 1};
        }
    } while (gamma != gamma_last and gamma.cost_sum != 0);
    return result_cycle;
}

/// Find a circuit, using a crude heuristic for low mean cost: a DFS visiting cheap edges first
std::optional<std::vector<Edge>>
MinimumMeanCycleCalculator::find_heuristically_good_circuit(std::vector<Edge> const& edges) const {
    std::set<NodeId> unvisited_nodes;
    // Build a quick&dirty graph, with edges sorted by weight
    std::map<NodeId, std::vector<NodeId>> edge_map;
    for (auto const& edge_id : edges) {
        std::array<NodeId, 2> ends{edge_id.first, edge_id.second};
        for (size_t i = 0; i < ends.size(); ++i) {
            unvisited_nodes.insert(ends[i]);
            edge_map[ends[i]].push_back(ends[1 - i]);
        }
    }
    for (auto&[from, edges] : edge_map) {
        std::sort(edges.begin(), edges.end(), [this, from = from](auto const& end_a, auto const& end_b) {
            return _graph.edge_cost(Edge{from, end_a}) < _graph.edge_cost(Edge{from, end_b});
        });
    }
    struct StackElement {
        NodeId current_node;
        size_t next_edge;
    };
    while (not unvisited_nodes.empty()) {
        auto const root = *unvisited_nodes.begin();
        unvisited_nodes.erase(unvisited_nodes.begin());
        std::vector<StackElement> stack{StackElement{root, 0}};
        std::set<NodeId> in_stack{root};
        while (not stack.empty()) {
            auto& stack_top = stack.back();
            auto const& edges_at_current = edge_map.at(stack_top.current_node);
            if (stack_top.next_edge >= edges_at_current.size()) {
                // Finished at current node
                in_stack.erase(stack_top.current_node);
                stack.pop_back();
                continue;
            }
            auto const& next = edges_at_current.at(stack_top.next_edge);
            ++stack_top.next_edge;
            // Do not go back to the previous node
            if (stack.size() > 1 and stack.at(stack.size() - 2).current_node == next) {
                continue;
            }
            if (in_stack.count(next)) {
                // Found a cycle => return it
                // We use a DFS, so any non-tree-edge is "inside" a path to the root of the tree
                std::vector<Edge> circuit{Edge{stack_top.current_node, next}};
                while (stack.back().current_node != next) {
                    circuit.emplace_back(stack.back().current_node, stack.at(stack.size() - 2).current_node);
                    stack.pop_back();
                }
                return circuit;
            } else {
                stack.push_back(StackElement{next, 0});
                unvisited_nodes.erase(next);
                in_stack.insert(next);
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
    return Gamma{total_cost, edges.size()};
}

bool MinimumMeanCycleCalculator::Gamma::operator==(MinimumMeanCycleCalculator::Gamma const& other) const {
    return cost_sum * other.num_edges == other.cost_sum * num_edges;
}

bool MinimumMeanCycleCalculator::Gamma::operator!=(MinimumMeanCycleCalculator::Gamma const& other) const {
    return not(*this == other);
}
}
