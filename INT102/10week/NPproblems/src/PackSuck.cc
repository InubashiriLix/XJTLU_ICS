
#include "PackSuck.hpp"

#include <iostream>
#include <vector>

std::vector<std::pair<int, int>> items = std::vector<std::pair<int, int>>(4);

void packSetup() {
    items[0] = std::pair(10, 60);
    items[1] = std::pair(20, 100);
    items[2] = std::pair(30, 120);
    items[3] = std::pair(45, 135);
}

void packSuckGreedy() {
    int restvol = 50;
    int val = 0;

    for (auto item : items) {
        if (restvol > item.first) {
            val += item.second;
            restvol -= item.first;
        }
    }
    std::cout << "done: with restvol " << restvol << " and value " << val << std::endl;
}
