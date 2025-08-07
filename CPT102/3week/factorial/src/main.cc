#include <iostream>

int fac(const int input) { return (input <= 1) ? 1 : input * fac(input - 1); }

long long facIter(int input) {
    long long rtn = 1;
    while (input >= 1) rtn *= (input--);
    return rtn;
}

int main() {
    std::cout << fac(4) << std::endl;
    std::cout << facIter(4) << std::endl;
    return 0;
}
