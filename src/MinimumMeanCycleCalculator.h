#ifndef MINIMUMMEANCYCLE_MINIMUMMEANCYCLECALCULATOR_H
#define MINIMUMMEANCYCLE_MINIMUMMEANCYCLECALCULATOR_H

#include "graph.hpp"

namespace MMC {

class MinimumMeanCycleCalculator {
public:
    explicit MinimumMeanCycleCalculator(Graph const& graph);

    double get_mmc_average_cost();

private:
    Graph const& _graph;
};

}

#endif //MINIMUMMEANCYCLE_MINIMUMMEANCYCLECALCULATOR_H
