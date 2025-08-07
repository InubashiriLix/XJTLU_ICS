#include <stdbool.h>
#include <stdio.h>

bool subset_sum(int *arr, int size, int target) {
  int maxm = 1 << size;
  for (int mask = 0; mask < size; mask++) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
      if (mask & (1 << i))
        sum += arr[i];
    }
    if (sum == target)
      return true;
  }
  return false;
}

int main() {
  int S[] = {3, 5, 6, 7}, n = 4, d = 15;
  if (subset_sum(S, n, d))
    printf("存在和为 %d 的子集\n", d);
  else
    printf("不存在\n");
  return 0;
}
