#include <cmath>
#include <iostream>

int main(int argc, char *argv[]) {
    int number;

    float P1x = 0.f;
    float P1y = 0.f;
    float P2x = 0.f;
    float P2y = 0.f;

    std::cout << "Enter the P1 x: ";
    std::cin >> P1x;

    std::cout << "Enter the P1 y: ";
    std::cin >> P1y;

    std::cout << "Enter the P2 x: ";
    std::cin >> P2x;

    std::cout << "Enter the P2 y: ";
    std::cin >> P2y;

    std::cout << "length: " << std::sqrt((P1x - P2x) * (P1x - P2x) + (P1y - P2y) * (P1y - P2y))
              << std::endl;

    std::cout << "gradient: " << (P2y - P1y) / (P2x - P1x) << std::endl;
}
