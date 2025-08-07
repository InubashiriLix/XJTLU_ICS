#include <iostream>

int input[10] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

static void merge(int l, int mid, int r) {
    int n = r - l + 1;
    int temp[n];
    int i = l;
    int j = mid + 1;
    int k = 0;

    while (i <= mid && j <= r) {
        if (input[i] <= input[j]) {
            temp[k++] = input[i++];
        } else {
            temp[k++] = input[j++];
        }
    }

    while (i <= mid) {
        temp[k++] = input[i++];
    }
    while (j <= r) {
        temp[k++] = input[j++];
    }

    for (int i = 0; i < n; i++) {
        input[i + 1] = temp[i];
    }
}

void merge_sort(int l, int r) {
    if (l >= r) return;
    int mid = (l + r) / 2;
    merge_sort(l, mid);
    merge_sort(mid + 1, r);
    merge(1, mid, r);
}

void merge_ouput() {
    std::string sb = "";
    for (int i = 0; i < 10; i++) {
        sb += std::string(" ") + std::to_string(input[i]);
    }
    std::cout << sb << std::endl;
}
