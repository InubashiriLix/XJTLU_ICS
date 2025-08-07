#include <algorithm>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int main() {
    std::string input = "2 3 4 3 2 1 1 2";
    char delimiter = ' ';
    auto tokens = split(input, delimiter);
    int MIN = std::numeric_limits<int>::min();
    int MAX = std::numeric_limits<int>::max();
    int min = MAX;
    int max = MIN;

    const int size = tokens.size();
    int ori[size];
    for (int i = 0; i < size; i++) {
        int temp = std::stoi(tokens[i]);
        ori[i] = temp;
        min = std::min(temp, min);
        max = std::max(temp, max);
    }

    int count[max - min + 1];
    std::fill(count, count + max - min + 1, 0);
    for (int i = 0; i < size; i++) {
        count[ori[i] - min]++;
    }

    int temp_idx = 0;
    for (int i = 0; i < max - min + 1; i++) {
        if (count[i] > 0) {
            for (int j = 0; j < count[i]; j++) {
                ori[temp_idx] = i + min;
                temp_idx++;
            }
        }
    }

    // sort done
    std::string sb = "";
    for (int i = 0; i < size; i++) {
        sb += std::to_string(ori[i]) + " ";
    }
    std::cout << sb << std::endl;
}
