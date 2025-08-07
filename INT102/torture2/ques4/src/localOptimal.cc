#include "localOptimal.hpp"

#include <algorithm>
#include <iostream>

int paneltyTable[4][4] = {{1, -3, -2, -3}, {-3, 1, -3, -2}, {-2, -3, 1, -3}, {-3, -2, -3, 1}};

int match(char a) {
    switch (a) {
        case 'A': {
            return 0;
        }
        case 'C': {
            return 1;
        }
        case 'T': {
            return 2;
        }
        case 'G': {
            return 3;
        }
        default: {
            throw std::runtime_error("over zone");
        }
    }
}

int getPanelty(const char A, const char B) {
    int temp_a = match(A);
    int temp_b = match(B);
    return paneltyTable[temp_a][temp_b];
}
