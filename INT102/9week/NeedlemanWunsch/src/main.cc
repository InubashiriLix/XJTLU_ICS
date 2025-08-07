#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

void NeedlemanWunsch(const std::string& A, const std::string& B) {
    const int MATCH = 1;
    const int MISMATCH = -1;
    const int GAP = -2;

    const int rows = A.size() + 1;
    const int cols = A.size() + 1;

    int map[rows][cols];
    std::fill(&map[0][0], &map[rows][cols] + rows * cols, 0);
    std::vector<std::vector<char>> traceback(rows, cols)

        for (int i = 0; i < cols; i++) map[0][i] = i * GAP;
    for (int i = 0; i < cols; i++) map[i][0] = i * GAP;

    for (int i = 1; i < rows; i++) {
        for (int j = 1; j < cols; j++) {
            int up = map[i - 1][j] + GAP;
            int left = map[i][j - 1] + GAP;
            int dial =
                (A[i - 1] == B[j - 1]) ? map[i - 1][j - 1] + MATCH : map[i - 1][j - 1] + MISMATCH;
            map[i][j] = std::max({up, left, dial});
            if (map[i][j] == dial) }
    }
}

int main() {
    std::cout << "Hell Here" << std::endl;
    return 0;
}
