#ifndef GRAPH_HPP
#define GRAPH_HPP

/**
   @file graph.hpp

   @brief This file provides a simple class @c Graph to model unweighted undirected graphs.
**/
#include <iosfwd>
#include <cstdint>
#include <limits>
#include <vector>
#include <functional>
#include <cassert>

namespace MMC {

using size_type = uint32_t;

using NodeId = size_type;

using EdgeWeight = int32_t;

using AccumulatedEdgeWeight = int64_t;

using Edge = std::pair<NodeId, NodeId>;

/**
   @class Graph

   This class models unweighted undirected graphs only.
   Edges (and edge costs) are stored as an adjacency matrix, if multiple parallel edges are added only the cheapest one
   is considered.
**/
class Graph {
public:
    /**
       @brief Creates a @c Graph with @c num_nodes isolated nodes.

       The number of nodes in the graph currently cannot be changed. You can only add edges between the existing nodes.
    **/
    explicit Graph(NodeId num_nodes);

    /** @return The number of nodes in the graph. **/
    [[nodiscard]] NodeId num_nodes() const;

    /**
       @brief Adds the edge <tt> {node1_id, node2_id} </tt> with specified weight.

       Checks that @c node1_id and @c node2_id are distinct and throws an exception otherwise.
       @warning Does not check that the edge does not already exist, so this class can be used to model non-simple graphs.
    **/
    void add_edge(Edge to_add, EdgeWeight weight);

    [[nodiscard]] EdgeWeight edge_cost(Edge const& edge_id) const;

    [[nodiscard]] bool edge_exists(Edge const& edge) const;

    /**
     * @brief Reads a graph in DIMACS format from the given istream and returns that graph.
     */
    static Graph read_dimacs(std::istream& str);

private:
    [[nodiscard]] size_t edge_id(Edge const& edge) const;

    std::vector<EdgeWeight> _edge_costs;
    std::vector<char> _edge_in_graph;
    size_type const _num_nodes;
}; // class Graph

inline NodeId Graph::num_nodes() const {
    return _num_nodes;
}

inline bool Graph::edge_exists(Edge const& edge) const {
    return _edge_in_graph[edge_id(edge)];
}

inline EdgeWeight Graph::edge_cost(Edge const& edge) const {
    assert(edge_exists(edge));
    return _edge_costs[edge_id(edge)];
}

inline size_t Graph::edge_id(Edge const& edge) const {
    return _num_nodes * edge.first + edge.second;
}

} // namespace MMC

#endif /* GRAPH_HPP */
