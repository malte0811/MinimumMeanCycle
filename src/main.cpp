#include <iostream>
#include <fstream>

#include "graph.hpp"
#include "blossomv/PerfectMatching.h"
#include "MinimumMeanCycleCalculator.h"

int main(int argc, char** argv) {
    using namespace MMC;
    if (argc != 3) {
        std::cout << "Expected exactly two arguments (path to input graph and path to output graph)!" << std::endl;
        return EXIT_FAILURE;
    }
    std::fstream input_file{argv[1]};
    if (input_file.fail()) {
        std::cout << "Failed to open the input file. Exiting." << std::endl;
        return EXIT_FAILURE;
    }
    std::ofstream output_file(argv[2], std::ios::out | std::ios::trunc);
    if (output_file.fail()) {
        std::cout << "Failed to open the output file. Exiting." << std::endl;
        return EXIT_FAILURE;
    }
    auto const graph = Graph::read_dimacs(input_file);

    MinimumMeanCycleCalculator calc(graph);
    auto const mmc = calc.find_mmc();
    output_file << "p edge " << graph.num_nodes() << ' ';
    if (mmc) {
        output_file << mmc->size() << '\n';
        for (auto const& edge : *mmc) {
            auto const ends = graph.edge_ends(edge);
            auto const cost = graph.edge_weight(edge);
            output_file << "e ";
            for (auto const end : ends) {
                output_file << (end.get() + 1) << ' ';
            }
            output_file << cost << '\n';
        }
    } else {
        output_file << "0\n";
    }
    output_file << std::flush;

    return EXIT_SUCCESS;
}
