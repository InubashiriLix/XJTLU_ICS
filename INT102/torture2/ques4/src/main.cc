#include <algorithm>
#include <iostream>
#include <string>

#include "localOptimal.hpp"
// #include "localOptimal.cc"
//
// FIXME: IT DOES NOT WORK

int main() {
    std::cout << "Hell Here" << std::endl;

    std::string A = "ACATGT";
    std::string B = "GAGT";

    std::cout << getPanelty('A', 'A') << std::endl;
    const int rows = 7;
    const int cols = 5;
    int map[rows][cols];

    std::fill(&map[0][0], &map[0][0] + rows * cols, 0);

    iterPSA(A, B, map);
    //
    // std::cout << getPanelty('A', '-') << std::endl;

    return 0;
}
