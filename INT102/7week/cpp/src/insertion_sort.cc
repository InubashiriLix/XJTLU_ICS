#include <iostream>
#include <string>

int input_insertion[10] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

void insertion_sort() {
    for (int i = 1; i < 10; i++) {
        if (input_insertion[i] < input_insertion[i - 1]) {
            int temp = input_insertion[i];
            for (int j = i; j > 0; j--) {
                if (input_insertion[j - 1] > temp) {
                    input_insertion[j] = input_insertion[j - 1];
                    input_insertion[j - 1] = temp;
                } else {
                    break;
                }
            }
        }
    }

    std::string sb = "";
    for (int i = 0; i < 10; i++) {
        sb += std::string(" ") + std::to_string(input_insertion[i]);
    }
    std::cout << sb << std::endl;
}
