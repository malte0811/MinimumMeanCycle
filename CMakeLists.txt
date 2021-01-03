cmake_minimum_required(VERSION 3.17)
project(MinimumMeanCycle)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Werror -fno-omit-frame-pointer -DDEBUG_OUTPUT")

set(CMAKE_CXX_STANDARD 17)

link_directories(libs)
link_libraries(blossom5)

add_executable(MinimumMeanCycle src/main.cpp src/graph.cpp src/graph.hpp src/blossomv/PerfectMatching.h
        src/blossomv/block.h src/TJoinCalculator.cpp src/TJoinCalculator.h src/ShortestPathCalculator.cpp
        src/ShortestPathCalculator.h src/heaps/binary_heap.hpp src/heaps/bag_heap.hpp src/heaps/fibo_heap.hpp
        src/heaps/heap.h
        src/heaps/heap_base.h src/MinimumMeanCycleCalculator.cpp src/MinimumMeanCycleCalculator.h src/TypesafeWrapper.h src/StepwisePathCalculator.cpp src/StepwisePathCalculator.h)
