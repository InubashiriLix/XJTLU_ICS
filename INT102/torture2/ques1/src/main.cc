#include <algorithm>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

struct Edge {
    int u;
    int v;
    int cost;
    Edge(int u_, int v_, int c_) : u(u_), v(v_), cost(c_){};
};

int node_num;
int edge_num;
std::vector<Edge> edges;

void BellmanFlod(int start) {
    int costs[node_num];
    const int MAX = std::numeric_limits<int>::max();
    std::fill(costs, costs + node_num, MAX);
    costs[start] = 0;
    for (int i = 0; i < node_num - 1; i++) {
        for (const auto& edge : edges) {
            if (costs[edge.u] < MAX && costs[edge.v] > costs[edge.u] + edge.cost) {
                costs[edge.v] = costs[edge.u] + edge.cost;
            }
        }
    }

    // TODO: check the negtive edge
    for (const auto& edge : edges) {
        if (costs[edge.u] != MAX && costs[edge.u] + edge.cost < costs[edge.v]) {
            std::cout << "Negative cycle detected" << std::endl;
            return;
        }
    }

    for (int i = 0; i < node_num; i++) {
        std::cout << std::to_string(costs[i]) + " ";
    }
    std::cout << std::endl;
}

int main() {
    std::string line1;

    std::getline(std::cin, line1);
    std::istringstream firstLine(line1);
    firstLine >> node_num >> edge_num;

    for (int i = 0; i < edge_num; i++) {
        int u, v, cost;
        std::string line;
        std::getline(std::cin, line);
        std::istringstream Line(line);
        Line >> u >> v >> cost;

        edges.push_back(Edge(u - 1, v - 1, cost));
    }

    BellmanFlod(0);

    return 0;
}
