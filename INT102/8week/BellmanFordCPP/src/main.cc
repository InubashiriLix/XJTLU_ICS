#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

struct Edge {
    int from;
    int to;
    int cost;
    Edge(int f, int t, int c) : from(f), to(t), cost(c) {}
};

void bell() {
    std::string input = "5 8\n1 2 6\n1 3 7\n2 3 8\n2 4 5\n2 5 -4\n3 4 -3\n3 5 9\n4 2 -2\n";
    std::istringstream iss(input);
    int n, m;
    iss >> n >> m;

    // std::vector<Edge> edges(m);
    std::vector<Edge> edges;
    int costs[n];
    const int INF = std::numeric_limits<int>::max();
    for (int i = 0; i < n; i++) {
        costs[i] = INF;
    }
    costs[0] = 0;

    for (int i = 0; i < m; i++) {
        int u, v, w;
        iss >> u >> v >> w;
        edges.push_back(Edge(u - 1, v - 1, w));
    }

    for (int i = 0; i < n; i++) {
        for (const auto &edge : edges) {
            if (costs[edge.from] != INF && costs[edge.from] + edge.cost < costs[edge.to]) {
                costs[edge.to] = costs[edge.from] + edge.cost;
            }
        }
    }

    for (const auto &edge : edges) {
        if (costs[edge.from] != INF && costs[edge.from] + edge.cost < costs[edge.to]) {
            std::cout << "Negative cycle detected" << std::endl;
            return;
        }
    }

    std::string sb(" ");
    for (int i = 0; i < n; i++) {
        sb += std::to_string(costs[i]) + " ";
    }
    std::cout << sb << std::endl;
}

int main() {
    bell();
    return 0;
}
