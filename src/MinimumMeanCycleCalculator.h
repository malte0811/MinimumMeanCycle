#ifndef MINIMUMMEANCYCLE_MINIMUMMEANCYCLECALCULATOR_H
#define MINIMUMMEANCYCLE_MINIMUMMEANCYCLECALCULATOR_H

#include "graph.h"
#include "Gamma.h"

namespace MMC {

class MinimumMeanCycleCalculator {
public:
    explicit MinimumMeanCycleCalculator(Graph const& graph);

    std::optional<std::pair<std::vector<Edge>, Gamma>> find_mmc();

private:
    [[nodiscard]] std::optional<std::vector<Edge>>
    find_heuristically_good_circuit(std::vector<Edge> const& edges) const;

    [[nodiscard]] Gamma get_average_cost(std::vector<Edge> const& edges) const;

    Graph const& _graph;
};

}

#endif //MINIMUMMEANCYCLE_MINIMUMMEANCYCLECALCULATOR_H
