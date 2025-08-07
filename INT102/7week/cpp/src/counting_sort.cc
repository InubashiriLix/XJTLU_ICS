#include "counting_sort.h"

#include <math.h>

#include <iostream>
#define DEBUG

void countingSort() {
    int input[5] = {4, 3, 3, 1, 1};
    int min = input[0];
    int max = input[0];

    // create the cnt array
    for (int i = 0; i < 5; i++) {
        min = std::min(input[i], min);
        max = std::max(input[i], max);
    }

    const int cnt_size = max - min + 1;  // size of the counting array
    int cnt[cnt_size];
    for (int i = 0; i < cnt_size; i++) {
        cnt[i] = 0;
    }

    for (int i = 0; i < 5; i++) {
        cnt[input[i] - min]++;
    }

    int temp_idx = 0;
    for (int i = 0; i < cnt_size; i++) {
        for (int j = 0; j < cnt[i]; j++) {
            input[temp_idx++] = i + min;  // i + min to get the original value
        }
    }

#ifdef DEBUG
    std::string sb = "";
    for (int i = 0; i < 5; i++) {
        sb += std::string(" ") + std::to_string(input[i]);
    }
    std::cout << sb << std::endl;
#endif  // DEBUG

    return;
}
