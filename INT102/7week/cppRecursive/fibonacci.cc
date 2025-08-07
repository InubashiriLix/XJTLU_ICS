
#include <stdio.h>

int fibonacci(int a, int b, int cnt) {
  if (cnt <= 0) {
    // std::cout << b << std::endl;
    return b;
  } else
    return fibonacci(b, a + b, cnt - 1);
}

int main() {
  // 1 1 2 3 5
  printf("result: %d", fibonacci(1, 1, 3));
  return 0;
}
