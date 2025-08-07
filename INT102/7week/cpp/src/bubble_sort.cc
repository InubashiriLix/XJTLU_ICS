
#include <stdio.h>

#include <iostream>

void bubble_sorting() {
    const int input_size = 10;
    int input[input_size] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    bool repeat_flag = 1;
    while (repeat_flag) {
        repeat_flag = 0;
        for (int i = 0; i < input_size - 1; i++) {
            if (input[i] > input[i + 1]) {
                repeat_flag = 1;
                int temp = input[i];
                input[i] = input[i + 1];
                input[i + 1] = temp;
            }
        }
    }

    std::string sb = "";
    for (int i = 0; i < 10; i++) {
        sb += std::string(" ") + std::to_string(input[i]);
    }

    std::cout << sb << std::endl;
}
