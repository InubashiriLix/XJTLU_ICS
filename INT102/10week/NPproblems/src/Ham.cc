#include <Ham.hpp>
#include <iostream>
#include <queue>
#include <vector>

int ham[8][8] = {
    {0, 1, 0, 0, 0, 0, 0, 1},  // A
    {1, 0, 1, 0, 0, 0, 1, 0},  // B
    {0, 1, 0, 1, 0, 0, 0, 0},  // C
    {0, 0, 1, 0, 1, 0, 0, 0},  // D
    {0, 0, 0, 1, 0, 1, 1, 0},  // E
    {0, 0, 0, 0, 1, 0, 0, 1},  // F
    {0, 1, 0, 0, 1, 0, 0, 0},  // G
    {1, 0, 0, 0, 0, 1, 0, 0}   // H
};

// 队列元素为路径和访问标记
bool HamiltonianCircuit() {
    // 队列中存储的每个元素是一个包含路径和访问标记的 pair
    std::queue<std::pair<std::vector<int>, std::vector<bool>>> q;

    // 从节点0开始
    std::vector<int> start_path = {0};
    std::vector<bool> visited(8, false);
    visited[0] = true;
    q.push({start_path, visited});

    while (!q.empty()) {
        auto current = q.front();
        q.pop();

        std::vector<int> path = current.first;
        std::vector<bool> visited = current.second;

        // 如果路径包含所有节点，并且最后一个节点与起始节点相连
        if (path.size() == 8) {
            if (ham[path.back()][path[0]] == 1) {
                std::cout << "Hamiltonian Circuit Found: ";
                for (int node : path) {
                    std::cout << char('A' + node) << " ";
                }
                std::cout << std::endl;
                return true;
            }
            continue;
        }

        // 尝试扩展路径
        for (int i = 0; i < 8; i++) {
            if (ham[path.back()][i] == 1 && !visited[i]) {
                path.push_back(i);
                visited[i] = true;
                q.push({path, visited});
                path.pop_back();     // 回退当前节点，继续其他路径探索
                visited[i] = false;  // 恢复访问状态
            }
        }
    }

    std::cout << "No Hamiltonian Circuit found" << std::endl;
    return false;
}
