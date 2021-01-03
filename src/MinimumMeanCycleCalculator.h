#ifndef MINIMUMMEANCYCLE_MINIMUMMEANCYCLECALCULATOR_H
#define MINIMUMMEANCYCLE_MINIMUMMEANCYCLECALCULATOR_H

#include "graph.hpp"

namespace MMC {

class MinimumMeanCycleCalculator {
public:
    explicit MinimumMeanCycleCalculator(Graph const& graph);

    std::optional<std::vector<Edge>> find_mmc();

private:
    [[nodiscard]] std::optional<std::vector<Edge>> find_any_circuit(std::vector<Edge> const& edges) const;

    [[nodiscard]] double get_average_cost(std::vector<Edge> const& edges) const;

    Graph const& _graph;
};

}

#endif //MINIMUMMEANCYCLE_MINIMUMMEANCYCLECALCULATOR_H
