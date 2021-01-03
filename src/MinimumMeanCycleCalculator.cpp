#include <iostream>
#include <cassert>
#include <cmath>
#include "MinimumMeanCycleCalculator.h"
#include "TJoinCalculator.h"

namespace MMC {

MinimumMeanCycleCalculator::MinimumMeanCycleCalculator(Graph const& graph) : _graph(graph) {}

double MinimumMeanCycleCalculator::get_mmc_average_cost() {
    double gamma = 0;
    //TODO std::max?
    //TODO use cost of some cycle as initial gamma instead? Easy to combine with checking for acyclic graphs
    for (EdgeId edge{0}; edge < _graph.num_edges(); ++edge) {
        auto const next_cost = _graph.edge_weight(edge);
        if (next_cost > gamma) {
            gamma = next_cost;
        }
    }
    double average_cost = 0;
    do {
        TJoinCalculator calc(_graph);
        std::cout << "Calculating join with gamma=" << gamma << '\n';
        auto const& min_join = calc.get_minimum_zero_join([gamma](double orig_cost) {
            return orig_cost - gamma;
        });
        if (not min_join.empty()) {
            double total_cost_current_cf = 0;
            for (auto const& join_edge : min_join) {
                total_cost_current_cf += _graph.edge_weight(join_edge) - gamma;
            }
            assert(total_cost_current_cf <= 0);
            average_cost = total_cost_current_cf / min_join.size();
            gamma += average_cost;
        } else {
            average_cost = 0;
        }
    } while (std::abs(average_cost) > 1e-3);
    return gamma;
}

}
