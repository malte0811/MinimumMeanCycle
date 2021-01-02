#include <iostream>
#include <fstream>

#include "graph.hpp"
#include "blossomv/PerfectMatching.h"
#include "MinimumMeanCycleCalculator.h"

int main(int /*argc*/, char** argv) {
    using namespace MMC;
    //if (argc != 3) {
    //    std::cout << "Expected exactly two arguments (path to input graph and path to output graph)!" << std::endl;
    //    return EXIT_FAILURE;
    //}
    std::fstream input_file{argv[1]};
    if (input_file.fail()) {
        std::cout << "Failed to open the input file. Exiting." << std::endl;
        return EXIT_FAILURE;
    }
    //std::ofstream output_file(argv[2], std::ios::out | std::ios::trunc);
    //if (output_file.fail()) {
    //    std::cout << "Failed to open the output file. Exiting." << std::endl;
    //    return EXIT_FAILURE;
    //}
    auto const graph = Graph::read_dimacs(input_file);

    MinimumMeanCycleCalculator calc(graph);
    std::cout << calc.get_mmc_average_cost() << '\n';

    return EXIT_SUCCESS;
}
