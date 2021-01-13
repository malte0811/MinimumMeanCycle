#include <iostream>
#include <fstream>

#include "graph.h"
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
    try {
        auto const graph = Graph::read_dimacs(input_file);

        MinimumMeanCycleCalculator calc(graph);
        auto const mmc_gamma_opt = calc.find_mmc();
        output_file << "p edge " << graph.num_nodes() << ' ';

        if (mmc_gamma_opt) {
            // Write edges of minimum mean cycle
            auto const&[mmc, gamma] = *mmc_gamma_opt;
            output_file << mmc.size() << '\n';
            for (auto const& edge : mmc) {
                output_file << "e ";
                for (auto const end : {edge.first, edge.second}) {
                    output_file << (end + 1) << ' ';
                }
                output_file << graph.edge_cost(edge) << '\n';
            }
        } else {
            // Graph is acyclic
            output_file << "0\n";
        }
        output_file << std::flush;

        return EXIT_SUCCESS;
    } catch (std::exception const& xcp) {
        std::cerr << "Caught exception: " << xcp.what() << '\n';
        return EXIT_FAILURE;
    }
}
