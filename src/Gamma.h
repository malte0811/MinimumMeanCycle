#ifndef MINIMUMMEANCYCLE_GAMMA_H
#define MINIMUMMEANCYCLE_GAMMA_H

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
    // Correct since num_edges is positive
    return cost_sum * other.num_edges < other.cost_sum * num_edges;
}

}

#endif //MINIMUMMEANCYCLE_GAMMA_H
