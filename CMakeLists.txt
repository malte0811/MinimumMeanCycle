cmake_minimum_required(VERSION 3.17)
project(MinimumMeanCycle)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Werror -fno-omit-frame-pointer -DDEBUG_OUTPUT")

set(CMAKE_CXX_STANDARD 17)

link_directories(libs)
link_libraries(blossom5)

add_executable(MinimumMeanCycle src/main.cpp src/graph.cpp src/graph.h src/blossomv/PerfectMatching.h
        src/blossomv/block.h src/TJoinCalculator.cpp src/TJoinCalculator.h src/ShortestPathCalculator.cpp
        src/ShortestPathCalculator.h
        src/binary_heap.h src/MinimumMeanCycleCalculator.cpp src/MinimumMeanCycleCalculator.h)
