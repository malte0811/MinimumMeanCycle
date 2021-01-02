#include "graph.hpp" // always include corresponding header first
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {

MMC::NodeId from_dimacs_id(MMC::size_type dimacs_node_id) {
    if (dimacs_node_id <= 0) {
        throw std::runtime_error("Non-positive DIMACS node id can not be converted.");
    }
    return dimacs_node_id - 1;
}

MMC::size_type to_dimacs_id(MMC::NodeId node_id) {
    return node_id + 1;
}

// Returns the first line which is not a comment, i.e. does not start with c.
std::string read_next_non_comment_line(std::istream& input) {
    std::string line;
    do {
        if (!std::getline(input, line)) {
            throw std::runtime_error("Unexpected end of DIMACS stream.");
        }
    } while (line[0] == 'c');
    return line;
}

} // end of anonymous namespace

namespace MMC {
/////////////////////////////////////////////
//! \c Node definitions
/////////////////////////////////////////////

void Node::add_outgoing_halfedge(HalfEdgeId const id) {
    _outgoing_halfedges.push_back(id);
}

/////////////////////////////////////////////
//! \c Graph definitions
/////////////////////////////////////////////

Graph::Graph(NodeId const num_nodes)
        : _nodes(num_nodes) {}

void Graph::add_edge(NodeId const node1_id, NodeId const node2_id, EdgeWeight const weight) {
    if (node1_id == node2_id) {
        throw std::runtime_error("MMC::Graph class does not support loops!");
    }

    HalfEdgeId const halfedge1_id = _halfedges.size();
    HalfEdgeId const halfedge2_id = halfedge1_id + 1;
    _nodes[node1_id].add_outgoing_halfedge(halfedge1_id);
    _nodes[node2_id].add_outgoing_halfedge(halfedge2_id);
    _halfedges.emplace_back(node2_id, halfedge2_id);
    _halfedges.emplace_back(node1_id, halfedge1_id);
    _edge_weights.emplace_back(weight);
}

Graph Graph::read_dimacs(std::istream& input) {
    std::string unused_word{};
    std::stringstream first_buffering_stream{};
    std::string const first_line = read_next_non_comment_line(input);

    if (first_line[0] != 'p') {
        throw std::runtime_error("Unexpected format of input file!");
    }

    size_type num_nodes{};
    size_type num_edges{};
    first_buffering_stream << first_line;
    first_buffering_stream >> unused_word >> unused_word >> num_nodes >> num_edges;

    // Now we successively add edges to our graph;
    Graph graph(num_nodes);
    for (size_type i = 1; i <= num_edges; ++i) {
        std::stringstream ith_buffering_stream{};
        std::string const ith_line = read_next_non_comment_line(input);
        size_type dimacs_node1{};
        size_type dimacs_node2{};
        EdgeWeight weight{};
        ith_buffering_stream << ith_line;
        ith_buffering_stream >> unused_word >> dimacs_node1 >> dimacs_node2 >> weight;
        graph.add_edge(from_dimacs_id(dimacs_node1), from_dimacs_id(dimacs_node2), weight);
    }

    return graph;
}

std::ostream& operator<<(std::ostream& output, Graph const& graph) {
    output << "p edge " << graph.num_nodes() << " " << graph.num_edges() << '\n';

    for (EdgeId edge_id{0}; edge_id < graph.num_edges(); ++edge_id) {
        output << "e " << to_dimacs_id(graph.halfedge(2 * edge_id + 1).target()) << " "
               << to_dimacs_id(graph.halfedge(2 * edge_id + 0).target()) << " "
               << graph.edge_weight(edge_id) << '\n';
    }
    output << std::flush;
    return output;
}

} // namespace MMC
