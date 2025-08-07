#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

void Warshell() {
    const int n = 4;
    int g[n][n] = {
        {0, 0, 1, 0},  // 1
        {1, 0, 0, 1},  // 2
        {0, 0, 0, 0},  // 3
        {0, 1, 0, 0}   // 4

    };

    for (int mid = 0; mid < n; mid++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (g[i][mid] == 1 && g[mid][j] == 1) g[i][j] = 1;
            }
        }
    }

    std::string sb("");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            sb += (std::to_string(g[i][j]) + " ");
        }
        sb += "\n";
    }
    std::cout << sb << std::endl;
}

int main() {
    Warshell();
    return 0;
}
