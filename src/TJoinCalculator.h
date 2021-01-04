#ifndef MINIMUMMEANCYCLE_TJOINCALCULATOR_H
#define MINIMUMMEANCYCLE_TJOINCALCULATOR_H

#include <functional>
#include "graph.hpp"

namespace MMC {

using TJoin = std::vector<Edge>;

class TJoinCalculator {
public:
    explicit TJoinCalculator(Graph const& baseGraph);

    [[nodiscard]] TJoin get_minimum_zero_join(std::function<long(long)> const& cost_transform) const;

    [[nodiscard]] TJoin get_minimum_cost_t_join(
            std::vector<NodeId> const& odd_nodes, std::function<long(long)> const& cost_transform
    ) const;

private:
    Graph const& _base_graph;
};

}

#endif //MINIMUMMEANCYCLE_TJOINCALCULATOR_H
