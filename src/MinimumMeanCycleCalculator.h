#ifndef MINIMUMMEANCYCLE_MINIMUMMEANCYCLECALCULATOR_H
#define MINIMUMMEANCYCLE_MINIMUMMEANCYCLECALCULATOR_H

#include "graph.h"

namespace MMC {

class MinimumMeanCycleCalculator {
public:
    explicit MinimumMeanCycleCalculator(Graph const& graph);

    std::optional<std::vector<Edge>> find_mmc();

private:
    struct Gamma {
        long cost_sum;
        size_t num_edges;

        bool operator==(Gamma const& other) const;

        bool operator!=(Gamma const& other) const;

        explicit operator double() const {
            return static_cast<double>(cost_sum) / static_cast<double>(num_edges);
        }
    };

    [[nodiscard]] std::optional<std::vector<Edge>>
    find_heuristically_good_circuit(std::vector<Edge> const& edges) const;

    [[nodiscard]] Gamma get_average_cost(std::vector<Edge> const& edges) const;

    Graph const& _graph;
};

}

#endif //MINIMUMMEANCYCLE_MINIMUMMEANCYCLECALCULATOR_H
