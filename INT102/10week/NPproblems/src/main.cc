#include <algorithm>
#include <iostream>

#include "Ham.hpp"
#include "PackSuck.hpp"

int main() {
    // HamiltonianCircuit();
    packSetup();
    // packSuckGreedy();
    packSuckDPsetup();
    int rtn = packSuckDP();
    std::cout << "the Dp result: " << rtn << std::endl;

    return 0;
}
