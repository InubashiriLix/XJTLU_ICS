#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

void Floyd() {
    const int INF = std::numeric_limits<int>::max();
    const int n = 4;
    int adjMatrix[n][n] = {
        {0, INF, -2, INF},  // 1
        {4, 0, 3, INF},     // 2
        {INF, INF, 0, 2},   // 3
        {INF, -1, INF, 0}   // 4
    };

    for (int k = 0; k < n; k++) {          // enumerate the intermediate nodes
        for (int i = 0; i < n; i++) {      // for the from node
            for (int j = 0; i < n; j++) {  // for the to node
                if (adjMatrix[i][j] > adjMatrix[i][k] + adjMatrix[k][j]) {
                    adjMatrix[i][j] = adjMatrix[i][k] + adjMatrix[k][j];
                }
            }
        }
    }
}

int main() { return 0; }
