#ifndef MINIMUMMEANCYCLE_TJOINCALCULATOR_H
#define MINIMUMMEANCYCLE_TJOINCALCULATOR_H

#include <functional>
#include "graph.h"
#include "MinimumMeanCycleCalculator.h"

namespace MMC {

using TJoin = std::vector<Edge>;

class TJoinCalculator {
public:
    explicit TJoinCalculator(Graph const& baseGraph);

    /// Calculate a minimum \emptyset-join with cost function cost_transform.apply(-)
    [[nodiscard]] TJoin get_minimum_zero_join(Gamma cost_transform) const;

    /// Calculate a minimum (odd_nodes)-join with cost function abs(cost_transform.apply(-))
    [[nodiscard]] TJoin get_minimum_cost_t_join_abs(std::vector<NodeId> const& odd_nodes, Gamma cost_transform) const;

private:
    Graph const& _base_graph;
};

}

#endif //MINIMUMMEANCYCLE_TJOINCALCULATOR_H
