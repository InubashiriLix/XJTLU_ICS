#include <iostream>

int fib(int cnt) {
    if (cnt == 1) return 1;
    if (cnt == 0) return 0;
    return fib(cnt - 1) + fib(cnt - 2);
}

int fibIter(int cnt) {
    int sum = 0;
    int a = 0;
    int b = 1;
    if (cnt == 1) return b;
    if (cnt == 0) return a;
    for (int i = 1; i < cnt; i++) {
        sum = a + b;

        a = b;
        b = sum;
    }
    return sum;
}

// 0 1 1 2 3 5
int main() {
    std::cout << fib(5) << std::endl;
    std::cout << fibIter(5) << std::endl;
    return 0;
}
