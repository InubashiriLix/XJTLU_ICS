#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

// 状态结构体：表示左右岸物品以及船夫位置
struct State {
    vector<char> left;   // 左岸物品
    vector<char> right;  // 右岸物品
    bool boat_left;      // 船夫位置：true在左岸，false在右岸
};

// 检查某一岸的状态是否安全：如果船夫不在该岸，羊不能和狼或菜一起
// 注意：此处只检查岸上情况，所以如果羊和狼同在，返回 false；羊和菜同在，也返回 false。
// 当羊不在该岸时，无论是否有狼或菜，都认为安全。
bool isSafe(const vector<char>& side) {
    bool hasW = (find(side.begin(), side.end(), 'W') != side.end());
    bool hasC = (find(side.begin(), side.end(), 'C') != side.end());
    bool hasG = (find(side.begin(), side.end(), 'G') != side.end());
    if (hasG && (hasW || hasC)) return false;
    return true;
}

// 将状态转换为字符串，用于记录已访问状态，避免死循环
string serializeState(const State& s) {
    string rep;
    rep += (s.boat_left ? "L" : "R");
    rep += "|";
    // 对左右岸内容进行排序以获得唯一表示
    vector<char> left = s.left;
    vector<char> right = s.right;
    sort(left.begin(), left.end());
    sort(right.begin(), right.end());
    for (char c : left) rep.push_back(c);
    rep += "|";
    for (char c : right) rep.push_back(c);
    return rep;
}

// 打印当前状态，便于观察过河过程
void printState(const State& s) {
    cout << "左岸: ";
    for (char c : s.left) cout << c << " ";
    if (s.boat_left) cout << " (船夫)";
    cout << "\n右岸: ";
    for (char c : s.right) cout << c << " ";
    if (!s.boat_left) cout << " (船夫)";
    cout << "\n----------------\n";
}

// 递归寻找过河解法
// 参数：当前状态、路径记录、已访问状态集合
bool findSolution(State current, vector<State>& path, unordered_set<string>& visited) {
    // 目标状态：左岸空且船夫在右岸（所有物品均在右岸）
    if (current.left.empty() && !current.boat_left) {
        path.push_back(current);
        return true;
    }
    string rep = serializeState(current);
    if (visited.find(rep) != visited.end()) return false;
    visited.insert(rep);

    // 根据船夫所在岸枚举所有可能的搬运操作
    if (current.boat_left) {
        // 船夫在左岸，从左岸向右岸移动
        // 操作1：单独搬运
        State next = current;
        next.boat_left = false;
        // 当船夫离开后，左岸处于无人看管状态，需要检查左岸是否安全
        if (isSafe(next.left)) {
            path.push_back(current);
            if (findSolution(next, path, visited)) return true;
            path.pop_back();
        }
        // 操作2：搭载左岸上任意一个物品搬运到右岸
        for (int i = 0; i < current.left.size(); i++) {
            next = current;
            char item = next.left[i];
            next.left.erase(next.left.begin() + i);  // 从左岸移除该物品
            next.right.push_back(item);              // 添加到右岸
            next.boat_left = false;                  // 船夫跟随一起过河
            if (isSafe(next.left)) {                 // 检查船夫离开后的左岸安全性
                path.push_back(current);
                if (findSolution(next, path, visited)) return true;
                path.pop_back();
            }
        }
    } else {
        // 船夫在右岸，从右岸向左岸返回
        // 操作1：单独回去
        State next = current;
        next.boat_left = true;
        if (isSafe(next.right)) {  // 右岸无人看管时检查安全性
            path.push_back(current);
            if (findSolution(next, path, visited)) return true;
            path.pop_back();
        }
        // 操作2：搭载右岸上任意物品返回左岸
        for (int i = 0; i < current.right.size(); i++) {
            next = current;
            char item = next.right[i];
            next.right.erase(next.right.begin() + i);
            next.left.push_back(item);
            next.boat_left = true;
            if (isSafe(next.right)) {
                path.push_back(current);
                if (findSolution(next, path, visited)) return true;
                path.pop_back();
            }
        }
    }
    return false;  // 无合法下一步时返回 false
}

int main() {
    // 初始状态：左岸有狼(W)、菜(C)和羊(G)，船夫在左岸；右岸为空
    State init;
    init.left = {'W', 'C', 'G'};
    init.right = {};
    init.boat_left = true;

    vector<State> solutionPath;
    unordered_set<string> visited;

    if (findSolution(init, solutionPath, visited)) {
        cout << "找到解决方案，总共经过 " << solutionPath.size() << " 步（状态变化）：\n";
        for (const auto& state : solutionPath) {
            printState(state);
        }
        // 最后打印目标状态
        printState(solutionPath.back());
    } else {
        cout << "没有找到解决方案。\n";
    }
    return 0;
}
