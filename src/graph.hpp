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

namespace MMC {

using size_type = uint32_t;

using NodeId = size_type;

using EdgeWeight = int32_t;
using EdgeId = size_type;
using HalfEdgeId = size_type;

class Graph;

/**
   @class Node

   @brief A @c Node stores an array of outgoing half edges (via their ids).

   @note The neighbors are not necessarily ordered, so searching for a specific neighbor takes O(degree)-time.
**/
class Node {
public:
    /** @brief Creates an isolated node (you can add neighbors later). **/
    Node() = default;

    /** @return The number of neighbors of this node. **/
    [[nodiscard]] size_type degree() const;

    /** @return The array of ids of the outgoing half edges of this node. **/
    [[nodiscard]] std::vector<HalfEdgeId> const& outgoing_halfedges() const;

private:
    friend Graph;

    /** @brief Adds @c id to the list of outgoing half edges of this node. */
    void add_outgoing_halfedge(HalfEdgeId id);

    std::vector<HalfEdgeId> _outgoing_halfedges;
}; // class Node

/**
   @class HalfEdge

   @brief One of the two halfs of some undirected edge.

   @note The half edge does not save its source,
   since we can look up the target of the inverse half edge.
*/
class HalfEdge {
public:
    /** @brief Do not allow creation of  a unititialized @c HalfEdge. */
    HalfEdge() = delete;

    /** @brief Create a @c HalfEdge with specified @c target and @c inverse. */
    HalfEdge(NodeId target, HalfEdgeId inverse);

    /** @return The id of this edge leads towards. */
    NodeId target() const;

    /** @return The id of the corresponding half edge in the opposite direction. */
    HalfEdgeId inverse() const;

private:
    NodeId _target;
    HalfEdgeId _inverse;
}; // class HalfEdge

/**
   @class Graph

   This class models unweighted undirected graphs only.
   Each edge is represented by two half edges.
   Both nodes and half edges are indexed using contiguous integers starting at 0.
   It is guaranteed all pairs of corresponding half edges follow each other directly.
   In particualar, you end up with the same number if you divide their ids by 2.
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

    /** @return The number of edges in the graph. **/
    [[nodiscard]] EdgeId num_edges() const;

    /** @return The number of half edges in the graph. It is twice the number of edges. */
    [[nodiscard]] HalfEdgeId num_halfedges() const;

    /** @return A reference to the node with the specified id. */
    [[nodiscard]] Node const& node(NodeId id) const;

    /** @return A reference to the half edge of this graph with the specified id. */
    [[nodiscard]] HalfEdge const& halfedge(HalfEdgeId id) const;

    /** @return The weight of the specified edge. */
    [[nodiscard]] EdgeWeight edge_weight(HalfEdgeId id) const;

    /** @return The weight of the specfiied half edge (i.e. the weight of its edge). */
    [[nodiscard]] EdgeWeight halfedge_weight(HalfEdgeId id) const;

    /**
       @brief Adds the edge <tt> {node1_id, node2_id} </tt> with specified weight.

       Checks that @c node1_id and @c node2_id are distinct and throws an exception otherwise.
       @warning Does not check that the edge does not already exist, so this class can be used to model non-simple graphs.
    **/
    void add_edge(NodeId node1_id, NodeId node2_id, EdgeWeight weight);

    [[nodiscard]] std::vector<Node> const& nodes() const;

    [[nodiscard]] double edge_cost(HalfEdgeId edge_id, std::function<double(double)> const& transform) const;

    /**
     * @brief Reads a graph in DIMACS format from the given istream and returns that graph.
     */
    static Graph read_dimacs(std::istream& str);

    /**
      @brief Prints the graph to the given ostream in DIMACS format.
    **/
    friend std::ostream& operator<<(std::ostream& str, Graph const& graph);

    [[nodiscard]] EdgeId edge_id(HalfEdgeId const& half_edge) const;

private:
    std::vector<Node> _nodes;
    std::vector<HalfEdge> _halfedges;
    std::vector<EdgeWeight> _edge_weights;
}; // class Graph

inline
size_type Node::degree() const {
    return outgoing_halfedges().size();
}

inline
std::vector<EdgeId> const& Node::outgoing_halfedges() const {
    return _outgoing_halfedges;
}

inline
HalfEdge::HalfEdge(NodeId const target, HalfEdgeId const inverse)
        : _target(target), _inverse(inverse) {}

inline
NodeId HalfEdge::target() const {
    return _target;
}

inline
HalfEdgeId HalfEdge::inverse() const {
    return _inverse;
}

inline
NodeId Graph::num_nodes() const {
    return _nodes.size();
}

inline
EdgeId Graph::num_edges() const {
    return _edge_weights.size();
}

inline
HalfEdgeId Graph::num_halfedges() const {
    return _halfedges.size();
}

inline
Node const& Graph::node(NodeId const id) const {
    return _nodes[id];
}

inline
HalfEdge const& Graph::halfedge(HalfEdgeId const id) const {
    return _halfedges[id];
}

inline
EdgeWeight Graph::edge_weight(EdgeId const id) const {
    return _edge_weights.at(id);
}

inline
EdgeWeight Graph::halfedge_weight(HalfEdgeId const id) const {
    return _edge_weights.at(edge_id(id));
}

inline std::vector<Node> const& Graph::nodes() const {
    return _nodes;
}

inline double Graph::edge_cost(HalfEdgeId edge_id, std::function<double(double)> const& transform) const {
    return transform(halfedge_weight(edge_id));
}

inline EdgeId Graph::edge_id(HalfEdgeId const& half_edge) const {
    return half_edge / 2;
}

} // namespace MMC

#endif /* GRAPH_HPP */
