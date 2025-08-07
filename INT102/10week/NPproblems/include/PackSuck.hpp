#pragma once

#include <vector>

struct Item {
    int weight;
    int value;
    int dense;
};

extern std::vector<std::pair<int, int>> items;
extern std::vector<Item> Items;
void packSetup(void);
void packSuckGreedy(void);
void packSuckDPsetup(void);
int packSuckDP(void);
