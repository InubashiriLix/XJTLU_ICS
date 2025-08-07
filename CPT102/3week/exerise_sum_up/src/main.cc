#include <exception>
#include <iostream>

const int arrayLen = 10;
const int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

int sum_up_recur(int cnt) {
    if (cnt >= 10) throw new std::out_of_range("Out of range");
    if (cnt == 0)
        return array[0];
    else
        return array[cnt] + sum_up_recur(cnt - 1);
}

int sum_up_iter(int cnt) {
    int sum = 0;
    for (int i = 0; i <= cnt; ++i) {
        sum += array[i];
    }
    return sum;
}

int main() {
    std::cout << sum_up_recur(9) << std::endl;
    std::cout << "Hell Here" << std::endl;
    return 0;
}
