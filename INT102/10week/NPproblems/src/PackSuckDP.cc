#include <iostream>
#include <vector>

#include "PackSuck.hpp"

std::vector<Item> Items(4);

void packSuckDPsetup() {
    for (int i = 0; i < 4; i++) {
        auto item = items[i];
        Items[i].weight = item.first;
        Items[i].value = item.second;
        Items[i].dense = item.second / item.first;
    }
}

int packSuckDP() {
    int W = 50;
    std::vector<Item> items = Items;
    int n = items.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(W + 1, 0));

    for (int i = 1; i <= n; i++) {
        for (int w = 1; w <= W; w++) {
            if (items[i - 1].weight <= w) {
                dp[i][w] =
                    std::max(dp[i - 1][w], dp[i - 1][w - items[i - 1].weight] + items[i - 1].value);
            } else {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }
    return dp[n][W];
}
