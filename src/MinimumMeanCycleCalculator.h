#ifndef MINIMUMMEANCYCLE_MINIMUMMEANCYCLECALCULATOR_H
#define MINIMUMMEANCYCLE_MINIMUMMEANCYCLECALCULATOR_H

#include "graph.h"

namespace MMC {

/// Used to store the value of gamma without running into issues with floating point accuracy
struct Gamma {
    AccumulatedEdgeWeight cost_sum;
    size_t num_edges;

    bool operator==(Gamma const& other) const;

    bool operator!=(Gamma const& other) const;

    bool operator<(Gamma const& other) const;

    explicit operator double() const;

    [[nodiscard]] AccumulatedEdgeWeight apply(EdgeWeight i) const;
};

class MinimumMeanCycleCalculator {
public:
    explicit MinimumMeanCycleCalculator(Graph const& graph);

    std::optional<std::vector<Edge>> find_mmc();

private:
    [[nodiscard]] std::optional<std::vector<Edge>>
    find_heuristically_good_circuit(std::vector<Edge> const& edges) const;

    [[nodiscard]] Gamma get_average_cost(std::vector<Edge> const& edges) const;

    Graph const& _graph;
};

inline bool Gamma::operator==(Gamma const& other) const {
    return cost_sum * other.num_edges == other.cost_sum * num_edges;
}

inline bool Gamma::operator!=(Gamma const& other) const {
    return not(*this == other);
}

inline Gamma::operator double() const {
    return static_cast<double>(cost_sum) / static_cast<double>(num_edges);
}

inline AccumulatedEdgeWeight Gamma::apply(EdgeWeight i) const {
    return i * static_cast<AccumulatedEdgeWeight>(num_edges) - cost_sum;
}

inline bool Gamma::operator<(Gamma const& other) const {
    // Correct since num_edges is nonnegative
    return cost_sum * other.num_edges < other.cost_sum * num_edges;
}

}

#endif //MINIMUMMEANCYCLE_MINIMUMMEANCYCLECALCULATOR_H
