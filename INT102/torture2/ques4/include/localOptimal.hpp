#pragma once
#include <algorithm>
#include <iostream>
#include <string>

extern int paneltyTable[4][4];
int getPanelty(const char A, const char B);

template <size_t rows, size_t cols>
void iterPSA(const std::string& A, const std::string& B, int (&map)[rows][cols]) {
    map[0][0] = 0;
    // for (int j = 1; j < A.size() + 1; j++) {
    //     // map[0][j] = map[0][j - 1] + getPanelty('-', B[j - 1]);
    //     map[0][j] = 0;
    // }
    // for (int i = 1; i < B.size() + 1; i++) {
    //     // map[i][0] = map[i - 1][0] + getPanelty(A[i - 1], '-');
    //     map[i][0] = 0;
    // }

    for (int i = 1; i < A.size() + 1; i++) {
        for (int j = 1; j < B.size() + 1; j++) {
            if (A[i - 1] == B[j - 1])
                map[i][j] = map[i - 1][j - 1] + getPanelty(A[i - 1], B[j - 1]);
            else {
                // int up = map[i - 1][j] + getPanelty(A[i - 1], '-');
                int up = map[i - 1][j];
                // int left = map[i][j - 1] + getPanelty('-', B[j - 1]);
                int left = map[i][j - 1];
                int dial = map[i - 1][j - 1] + getPanelty(A[i - 1], B[j - 1]);
                map[i][j] = std::min({up, dial, left});
            }
        }
    }

    for (int i = 0; i < A.size() + 1; i++) {
        for (int j = 0; j < B.size() + 1; j++) {
            std::cout << map[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
