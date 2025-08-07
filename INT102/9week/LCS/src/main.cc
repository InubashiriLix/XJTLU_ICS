#include <iostream>
#include <string>

void nor(const std::string A, const std::string B) {
    int Alen = A.length();
    int Blen = B.length();
    int map[Alen + 1][Blen + 1];

    for (int i = 0; i < Alen + 1; i++) {
        for (int j = 0; j < Blen + 1; j++) {
            map[i][j] = 0;
        }
    }

    std::string sb = "";
    for (int i = 1; i < Alen + 1; i++) {
        for (int j = 1; j < Blen + 1; j++) {
            if (A[i - 1] == B[j - 1]) {
                map[i][j] = map[i - 1][j - 1] + 1;
                sb += A[i - 1];
            }

            else
                map[i][j] = std::max(map[i - 1][j], map[i][j - 1]);
        }
    }

    for (int i = 0; i < Alen + 1; i++) {
        for (int j = 0; j < Blen + 1; j++) {
            std::cout << std::to_string(map[i][j]) + " ";
        }
        std::cout << "" << std::endl;
    }
    std::cout << sb << std::endl;
}

std::string recurSb = "";

int recurLCS(const std::string A, const std::string B, int idxA, int idxB) {
    if (idxA == 0 || idxB == 0) {
        return 0;
    }
    if (A[idxA - 1] == B[idxB - 1]) {
        return 1 + recurLCS(A, B, idxA - 1, idxB - 1);
    } else {
        return std::max(recurLCS(A, B, idxA - 1, idxB), recurLCS(A, B, idxA, idxB - 1));
    }
}

int main() {
    std::string A = "ABCLS";
    std::string B = "ACSO";
    // nor(A, B);
    std::cout << recurLCS(A, B, A.length(), B.length()) << std::endl;
}
