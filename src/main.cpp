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
        auto const[graph, parallel_edge_cycle] = Graph::read_dimacs(input_file);

        MinimumMeanCycleCalculator calc(graph);
        auto const mmc_gamma_opt = calc.find_mmc();
        output_file << "p edge " << graph.num_nodes() << ' ';

        if (parallel_edge_cycle) {
            Gamma const parallel_gamma{parallel_edge_cycle->weight_1 + parallel_edge_cycle->weight_2, 2};
            if (not mmc_gamma_opt or parallel_gamma < mmc_gamma_opt->second) {
                // Special case: two parallel edges form the cheapest cycle
                output_file << "2\n";
                for (auto const cost : {parallel_edge_cycle->weight_1, parallel_edge_cycle->weight_2}) {
                    for (auto const end : {parallel_edge_cycle->end_a, parallel_edge_cycle->end_b}) {
                        output_file << (end + 1) << ' ';
                    }
                    output_file << cost << '\n';
                }
                return EXIT_SUCCESS;
            }
        }

        if (mmc_gamma_opt) {
            // Found "regular" (length >= 3) cheapest cycle
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
