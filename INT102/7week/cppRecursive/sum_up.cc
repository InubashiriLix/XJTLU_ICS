#include <stdio.h>

static const int arr_size = 10;
static int shitarray[arr_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

int recur_sum_up(int a, int cnt) {
  if (cnt >= arr_size) {
    return a;
  } else {
    return recur_sum_up(a + shitarray[cnt], cnt + 1);
  }
}

int main() {
  int result = recur_sum_up(0, 0);
  printf("shit result: %d", result);
  return 0;
}
