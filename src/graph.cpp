#include "graph.h" // always include corresponding header first
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace MMC {

namespace {

NodeId from_dimacs_id(size_type dimacs_node_id) {
    if (dimacs_node_id <= 0) {
        throw std::runtime_error("Non-positive DIMACS node id can not be converted.");
    }
    return NodeId(dimacs_node_id - 1);
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

/////////////////////////////////////////////
//! \c Graph definitions
/////////////////////////////////////////////

Graph::Graph(NodeId const num_nodes) :
        _edge_costs(num_nodes * num_nodes),
        _edge_in_graph(num_nodes * num_nodes),
        _num_nodes(num_nodes) {}

void Graph::add_edge(Edge const to_add, EdgeWeight const weight) {
    if (to_add.first == to_add.second) {
        throw std::runtime_error("MMC::Graph class does not support loops!");
    }
    if (edge_exists(to_add)) {
        throw std::runtime_error("MMC::Graph class does not support parallel edges!");
    }
    for (auto const& e : {to_add, std::make_pair(to_add.second, to_add.first)}) {
        _edge_in_graph.at(edge_id(e)) = true;
        _edge_costs.at(edge_id(e)) = weight;
    }
}

Graph Graph::read_dimacs(std::istream& input) {
    std::string unused_word{};
    std::string const first_line = read_next_non_comment_line(input);

    if (first_line[0] != 'p') {
        throw std::runtime_error("Unexpected format of input file!");
    }

    size_type num_nodes{};
    size_type num_edges{};
    std::stringstream first_buffering_stream{first_line};
    first_buffering_stream >> unused_word >> unused_word >> num_nodes >> num_edges;
    if (not first_buffering_stream) {
        throw std::runtime_error("Invalid first line in DIMACS: " + first_line);
    }

    // Now we successively add edges to our result_graph;
    Graph result_graph(NodeId{num_nodes});
    for (size_type i = 1; i <= num_edges; ++i) {
        std::string const ith_line = read_next_non_comment_line(input);
        size_type dimacs_node1{};
        size_type dimacs_node2{};
        EdgeWeight weight{};
        std::stringstream ith_buffering_stream{ith_line};
        ith_buffering_stream >> unused_word >> dimacs_node1 >> dimacs_node2 >> weight;
        if (not ith_buffering_stream or
            dimacs_node1 < 1 or dimacs_node1 > num_nodes or
            dimacs_node2 < 1 or dimacs_node2 > num_nodes) {
            throw std::runtime_error("Invalid data in DIMACS line: " + ith_line);
        }
        auto const node1 = from_dimacs_id(dimacs_node1);
        auto const node2 = from_dimacs_id(dimacs_node2);
        result_graph.add_edge({node1, node2}, weight);
    }
    return result_graph;
}

} // namespace MMC
