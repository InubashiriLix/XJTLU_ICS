#include <algorithm>
#include <iostream>

void PSA(const std::string A, const std::string B) {
    int lenA = A.length();
    int lenB = B.length();

    int reward = 1;
    int mismatch = -1;
    int blank_panel = -1;

    int map[lenA + 1][lenB + 1];
    std::fill(&map[0][0], &map[0][0] + (lenA + 1) * (lenB + 1), 0);
    for (int j = 0; j < lenB; j++) map[0][j] = j * blank_panel;
    for (int i = 0; i < lenA; i++) map[i][0] = i * blank_panel;

    std::string sb = "";
    for (int i = 1; i < lenA + 1; i++) {
        for (int j = 1; j < lenB + 1; j++) {
            if (A[i - 1] == B[j - 1]) {
                map[i][j] = map[i - 1][j - 1] + reward;
                sb += A[i - 1];
            } else {
                int dial = map[i - 1][j - 1] + mismatch;
                int left = map[i][j - 1] + blank_panel;
                int up = map[i - 1][j] + blank_panel;
                map[i][j] = std::max({dial, left, up});
            }
        }
    }

    for (int i = 0; i < lenA + 1; i++) {
        for (int j = 0; j < lenB + 1; j++) {
            std::cout << std::to_string(map[i][j]) + " ";
        }
        std::cout << "" << std::endl;
    }
    std::cout << sb << std::endl;
}

int main() {
    std::cout << "Hell Here" << std::endl;

    std::string A = "ABCLS";
    std::string B = "ACSO";
    PSA(A, B);

    return 0;
}
