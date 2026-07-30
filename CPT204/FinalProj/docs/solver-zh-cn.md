# Task B：图路径算法中文详解版

## 1. 任务范围

Task B 使用 Task A 选出的候选地点和 `paths.csv` 中的基础设施路径数据来回答路线查询。`paths.csv` 被建模为无向加权图：

- 每个地点是一个图节点；
- CSV 中每一行是一条无向边；
- 边上的 `weight` 表示通行成本。

当前图包含 1000 个地点和 2600 条无向边。程序读取时会把每条无向边存成两个方向，所以内存中的邻接表有 5200 条有向边记录。

正式路线结果使用 `DijkstraSolver` 生成。Dijkstra 适合本项目，因为本课程图中的边权都是非负数。项目中还实现了其他精确算法、近似算法和实验性算法，用于性能对比和 GUI 展示。

## 2. 图结构和共享路线逻辑

### Graph

`Graph` 使用邻接表保存图：

```text
Map<String, List<Edge>> adjacencyList
```

邻接表适合最短路径算法，因为算法经常需要查询“当前节点可以走到哪些邻居”。用邻接表时，只需要读取当前节点对应的边列表，不需要每次扫描全图所有边。

### PathResult

每个 solver 返回一个 `PathResult`，里面包含：

- `path`：按顺序排列的地点 ID；
- `totalCost`：路径上所有边权之和；
- `reachable`：目标是否可达。

### RouteQueryService

`RouteQueryService` 对 solver 做了一层封装，支持两类查询：

- direct query：从起点直接到终点；
- via query：从起点经过一个或多个途经点，再到终点。

对于 via query，程序会按照指定顺序逐段求最短路径，然后把每段路径拼接起来。拼接第二段及后续段时，会去掉该段的第一个节点，避免途经点重复出现两次。

伪代码：

```text
route = 空列表
totalCost = 0
current = start

for next in waypoints 加 destination:
    segment = solver.shortestPath(current, next)
    if segment 不可达:
        return unreachable

    if route 为空:
        加入 segment.path 的全部节点
    else:
        加入 segment.path 中除第一个节点以外的节点

    totalCost += segment.totalCost
    current = next

return PathResult(route, totalCost, reachable=true)
```

这表示系统会优化每一段路线，但不会重新排列途经点顺序。如果题目要求先经过 B5 再经过 C5，系统就按这个顺序执行。

## 3. 四个路线用例结果

以下正式结果来自 `output/taskB_route_results_20260518_220459.csv`。

| Case   | 起点  | 终点  | 途经点       | 是否可达 | 总成本 |
| ------ | ----- | ----- | ------------ | -------- | -----: |
| Case 1 | L0001 | L0001 | None         | true     |    0.0 |
| Case 2 | L0001 | L0010 | None         | true     |   27.0 |
| Case 3 | L0001 | L0101 | L0105        | true     |   39.0 |
| Case 4 | L0001 | L0201 | L0105, L0205 | true     |   48.0 |

路径如下：

```text
Case 1:
L0001

Case 2:
L0001 -> L0340 -> L0339 -> L0895 -> L0894 -> L0082 -> L0284 -> L0010

Case 3:
L0001 -> L0340 -> L0339 -> L0247 -> L0017 -> L0128 -> L0107 -> L0106 -> L0105
      -> L0106 -> L0107 -> L0108 -> L0827 -> L0996 -> L0101

Case 4:
L0001 -> L0340 -> L0339 -> L0247 -> L0017 -> L0128 -> L0107 -> L0106 -> L0105
      -> L0106 -> L0107 -> L0108 -> L0243 -> L0242 -> L0241 -> L0385 -> L0205 -> L0201
```

Case 3 和 Case 4 中有节点被重复经过，这是由固定途经点顺序造成的，不是程序错误。系统先求到途经点的最短路，再从途经点继续求下一段最短路。

## 4. 算法性能对比结果

以下平均时间来自 `output/taskB_algorithm_epoch_times_20260518_220500.csv`。每个算法都对四个路线用例运行 20 次。

| 算法                                     | 平均查询时间 ns | 预处理时间 ns | 是否与 Dijkstra 成本相同 |
| ---------------------------------------- | --------------: | ------------: | ------------------------ |
| Q-Learning Routing (experimental)        |           57225 |     264296885 | true                     |
| Greedy Best-First Learned (experimental) |          116416 |      69363145 | false                    |
| Greedy Best-First Landmark (approximate) |          402202 |      26665617 | false                    |
| A\* Learned Heuristic (experimental)     |          530709 |      69363145 | true                     |
| Unweighted BFS (approximate)             |          994122 |             0 | false                    |
| A\* Landmark                             |         1234577 |      26665617 | true                     |
| Heap Dijkstra (PriorityQueue)            |         2242236 |             0 | true                     |
| Bidirectional Dijkstra                   |         3978780 |             0 | true                     |
| SPFA                                     |         5495040 |             0 | true                     |
| Bellman-Ford                             |        10568929 |             0 | true                     |
| Linear Dijkstra                          |        18617508 |             0 | true                     |

这个表不能只看查询时间。部分算法是精确最短路，能保证和 Dijkstra 一样的最优成本；部分算法是近似算法，速度快但可能不是最短路。Q-learning 的查询时间很低，但它有很大的训练成本，所以不能说它整体没有代价。

## 5. DijkstraSolver

### 算法说明

`DijkstraSolver` 是项目的主要正式 solver。它适用于非负权加权图。算法会维护从起点到每个节点的当前最短已知距离，并不断选择距离起点最近的未处理节点进行扩展。

项目实现使用 `PriorityQueue`，因此可以较快取出当前距离最小的节点。当发现到某个邻居的更短路线时，程序会更新该邻居的距离，并在 `previous` 表中记录它的前驱节点。

### 伪代码

```text
if start == destination:
    return [start], cost = 0

所有 distance 初始化为 infinity
distance[start] = 0
把 (start, 0) 放入优先队列

while 队列不为空:
    current = 取出距离最小的节点
    if current 是旧队列项:
        continue
    if current 是 destination:
        break

    for current 的每条出边 current -> neighbour:
        newDistance = distance[current] + edge.weight
        if newDistance < distance[neighbour]:
            distance[neighbour] = newDistance
            previous[neighbour] = current
            把 neighbour 加入队列

if destination 距离仍为 infinity:
    return unreachable
else:
    根据 previous 重建路径
```

### 路径构造

算法从终点开始，不断查 `previous[current]`，直到回到起点。这个过程得到的是反向路径，所以最后需要 reverse。

### 复杂度

设 V 为节点数，E 为邻接表中的有向边记录数。

- 时间复杂度：O((V + E) log V)。
- 空间复杂度：O(V + E)。距离表和前驱表是 O(V)，优先队列中可能有旧条目，所以队列最坏可接近 O(E)。

### 为什么适合本项目

本项目图是非负权加权图，Dijkstra 正好适合这种场景。它能返回精确最短路径，并且对当前数据规模足够高效。

## 6. LinearDijkstraSolver

### 算法说明

`LinearDijkstraSolver` 使用和 Dijkstra 相同的松弛思想，但不使用堆。每一轮它都会线性扫描所有未访问节点，找出距离起点最近的节点。

这个版本更容易理解，但在大图上更慢。它主要用于和堆优化 Dijkstra 对比，说明 `PriorityQueue` 的价值。

### 伪代码

```text
所有 distance 初始化为 infinity
distance[start] = 0
visited = 空集合

while 还有未访问节点:
    current = 未访问节点中 distance 最小的节点
    if current 不存在或 current 是 destination:
        break

    标记 current 已访问
    for current 的每条边:
        如果邻居未访问，则尝试松弛

根据 previous 重建路径
```

### 路径构造

和堆优化 Dijkstra 相同，使用 `previous` 从终点回溯到起点，再反转列表。

### 复杂度

- 时间复杂度：O(V^2 + E)。其中 O(V^2) 来自每轮扫描所有节点找最近未访问点。
- 空间复杂度：O(V)。

## 7. BellmanFordSolver

### 算法说明

Bellman-Ford 会反复松弛所有边。最多经过 V - 1 轮后，如果不存在负权环，所有最短路径都会被找到。

在本项目中，图的权重非负，所以 Bellman-Ford 不是最高效选择。但它有理论价值：它可以处理一般 Dijkstra 不能处理的负权边。

### 伪代码

```text
收集所有有向边
所有 distance 初始化为 infinity
distance[start] = 0

重复 V - 1 次:
    updated = false
    for 每条边 u -> v:
        if distance[u] + weight(u, v) < distance[v]:
            distance[v] = distance[u] + weight(u, v)
            previous[v] = u
            updated = true
    if 没有更新:
        break

根据 previous 重建路径
```

### 路径构造

每次松弛成功时，记录 `previous[v] = u`。最后从终点回溯到起点。

### 复杂度

- 时间复杂度：O(VE)。
- 空间复杂度：O(V + E)，因为实现中收集了所有边，并保存距离表和前驱表。

## 8. SpfaSolver

### 算法说明

SPFA 是 Bellman-Ford 的队列优化版本。它不会每轮扫描全部边，而是只重新处理最近距离发生变化的节点。某个节点距离更新后，会被放入队列，之后再用它的出边去更新邻居。

SPFA 在很多实际图上比 Bellman-Ford 快，但最坏时间复杂度仍然是 O(VE)。对于本项目这种非负权图，它不如 Dijkstra 稳定。

### 伪代码

```text
所有 distance 初始化为 infinity
distance[start] = 0
queue = [start]
inQueue = {start}

while queue 不为空:
    current = queue.remove()
    从 inQueue 中移除 current

    for current 的每条边:
        if distance[current] + weight < distance[neighbour]:
            更新 distance 和 previous
            if neighbour 不在队列:
                加入队列

根据 previous 重建路径
```

### 路径构造

SPFA 和 Bellman-Ford 一样，在每次成功松弛时记录前驱节点。

### 复杂度

- 最坏时间复杂度：O(VE)。
- 空间复杂度：O(V)。

## 9. UnweightedBreadthFirstSolver

### 算法说明

`UnweightedBreadthFirstSolver` 使用 BFS，并在搜索时忽略边权。BFS 找到的是边数最少的路径，也就是 hop count 最少的路径，而不一定是总权重最低的路径。

找到路径后，代码会再计算这条路径的真实加权成本，方便和其他算法输出比较。但这个成本不一定是最小成本。

### 伪代码

```text
queue = [start]
visited = {start}

while queue 不为空:
    current = queue.remove()
    if current 是 destination:
        break

    for current 的每个邻居:
        if 邻居未访问:
            visited.add(邻居)
            previous[邻居] = current
            queue.add(邻居)

如果 destination 被访问:
    重建路径，并累加真实边权
else:
    return unreachable
```

### 路径构造

BFS 记录第一次到达每个节点的前驱，因此得到的是最少边数路径。之后再沿路径累加边权，得到 `totalCost`。

### 复杂度

- 时间复杂度：O(V + E)。
- 空间复杂度：O(V)。

### 局限

只有当图是无权图，或者所有边权相同时，BFS 才能保证成本最优。在当前加权图中，它只是近似对照算法。

## 10. AStarSolver

### 算法说明

A\* 是 Dijkstra 的启发式扩展。它不仅考虑从起点到当前节点的已知成本，还考虑当前节点到终点的估计剩余成本：

```text
f(n) = g(n) + h(n)
```

其中：

- `g(n)` 是从起点到 n 的已知距离；
- `h(n)` 是从 n 到终点的估计距离。

如果启发式函数不会高估真实剩余距离，A\* 仍然能保证最短路径，同时可能比 Dijkstra 扩展更少节点。

### 伪代码

```text
distance[start] = 0
把 start 以 priority = heuristic(start, destination) 加入队列

while 队列不为空:
    current = 取出估计总成本最小的节点
    if current 是旧条目:
        continue
    if current 是 destination:
        break

    for current 的每条边:
        newDistance = distance[current] + edge.weight
        if newDistance < distance[neighbour]:
            distance[neighbour] = newDistance
            previous[neighbour] = current
            priority = newDistance + heuristic(neighbour, destination)
            加入队列

根据 previous 重建路径
```

### 路径构造

A\* 和 Dijkstra 一样使用前驱表 `previous` 重建路径。

### 复杂度

- 最坏时间复杂度：O((V + E) log V)。
- 空间复杂度：O(V + E)。

如果启发式质量好，实际扩展节点数可能明显减少。

## 11. LandmarkHeuristic

### 算法说明

`LandmarkHeuristic` 是 ALT 风格的启发式方法。它选择若干 landmark 节点，预先计算每个 landmark 到所有节点的最短距离，然后利用三角不等式得到下界：

```text
h(current, target) = max over landmarks |dist(landmark, target) - dist(landmark, current)|
```

对于无向非负权图，这个值是安全下界，因此 A\* 使用它仍能保证最短路径。

### 伪代码

```text
预处理:
    for 每个 landmark:
        用 Dijkstra 计算 landmark 到所有节点的距离
        保存距离表

估计 current 到 target:
    best = 0
    for 每个 landmark 距离表:
        bound = abs(dist[landmark][target] - dist[landmark][current])
        best = max(best, bound)
    return best
```

### 复杂度

设 L 为 landmark 数量。

- 预处理时间：O(L(E + V) log V)。
- 单次估计时间：O(L)。
- 空间复杂度：O(LV)。

### 实验解释

最新实验中，A\* Landmark 的预处理时间为 26665617 ns，平均查询时间为 1234577 ns，并且路径成本与 Dijkstra 相同。

## 12. LearnedHeuristic

### 算法说明

`LearnedHeuristic` 是实验性的 Java 线性回归启发式。它使用几个特征：

- 两个 location ID 数字部分的距离；
- 两个节点是否在同一个 hundred block；
- landmark lower bound；
- ID 距离和 landmark lower bound 的交互项。

模型使用 landmark 到节点的距离作为训练数据。和 LandmarkHeuristic 不同，学习出来的估计值不保证一定低估真实距离。如果它高估，A\* 的理论最优性可能被破坏。因此项目会用 `same_route_costs` 和 Dijkstra 结果对比。

### 伪代码

```text
预计算 landmark 距离表
用 landmark 到节点的真实距离构造训练样本

for 每个训练 epoch:
    for 每个样本:
        predicted = dot(weights, features)
        error = predicted - target
        用梯度下降更新 weights

estimate(current, target):
    prediction = dot(weights, features(current, target)) * scale
    if prediction 无效或小于 0:
        return 0
    return prediction
```

### 复杂度

设 L 为 landmark 数，S 为训练样本数，T 为训练轮数，F 为特征数量。

- 预处理时间：O(L(E + V) log V + TSF)。
- 单次估计时间：O(F + L)。
- 空间复杂度：O(LV + F)。

### 实验解释

最新实验中，A\* Learned Heuristic 预处理时间为 69363145 ns，平均查询时间为 530709 ns，并且四个测试用例中成本与 Dijkstra 相同。但它仍然是实验性方法，因为理论上不能保证所有输入都最优。

## 13. GreedyBestFirstSolver

### 算法说明

Greedy Best-First Search 只使用启发式值作为优先级。它不考虑从起点到当前节点已经花了多少成本。因此它会优先走向“看起来离终点近”的节点，即使已经走过的路径成本很高。

这种方法可能很快，但不保证加权最短路径。

### 伪代码

```text
queue = start，按 heuristic 排序
visited = 空集合

while queue 不为空:
    current = 取出 heuristic 最小的节点
    if current 已访问:
        continue
    标记 current 已访问

    if current 是 destination:
        重建路径并计算真实边权成本

    for 每个邻居:
        if 邻居未访问:
            如果 previous 中没有该邻居，则记录前驱
            按 heuristic 加入队列

return unreachable
```

### 路径构造

Greedy 记录第一次发现节点时的前驱。到达终点后，回溯前驱表得到路径，再累加实际边权。

### 复杂度

- 时间复杂度：通常为 O((V + E) log V)。
- 空间复杂度：O(V + E)。

### 局限

Greedy 是近似算法。最新实验中两个 Greedy 版本的 `same_route_costs` 都是 false，说明它们并不总是得到 Dijkstra 的最优成本。

## 14. BidirectionalDijkstraSolver

### 算法说明

双向 Dijkstra 同时运行两个搜索：

- 从起点向终点搜索；
- 从终点向起点搜索。

因为本项目图是无向图，所以两个方向都可以使用同一个邻接表。算法会记录两个搜索相遇的节点，并维护目前发现的最佳总距离。当两个搜索前沿的最小可能距离已经不可能优于当前最佳距离时，算法停止。

### 伪代码

```text
forward 从 start 开始
backward 从 destination 开始
bestDistance = infinity
meeting = none

while 两个队列都不为空:
    forward 扩展一个节点
    如果该节点在 backward 中有距离，更新 bestDistance

    backward 扩展一个节点
    如果该节点在 forward 中有距离，更新 bestDistance

    if forwardQueueMin + backwardQueueMin >= bestDistance:
        break

如果找到 meeting:
    拼接 forward 路径和 backward 路径
else:
    return unreachable
```

### 路径构造

forward 记录从起点来的前驱，backward 记录从终点方向来的前驱。重建时，先从 meeting 回溯到起点并反转，再从 meeting 沿 backward 前驱走到终点。

### 复杂度

- 最坏时间复杂度：O((V + E) log V)。
- 空间复杂度：O(V + E)，因为要维护两套距离、前驱、settled 集合和队列。

在大图、起终点距离较远时，它可能减少搜索节点。但在小图中，额外维护两套状态也会带来开销。

## 15. QLearningRouteSolver

### 算法说明

`QLearningRouteSolver` 是实验性的强化学习路线算法。它为每个目标节点学习一张 Q-table。Q 值表示“为了到达某个目标，从当前节点走向某个邻居”的好坏。

奖励值设置为负边权，因此低成本边会得到更好的反馈。训练使用 epsilon-greedy：

- 以 `EPSILON` 概率随机选边进行探索；
- 否则选择当前 Q 值最高的边。

训练完成后，查询时从起点开始，按学到的 Q 值贪心选择下一条边，并尽量避免重复访问节点。

### 伪代码

```text
train(graph, destinations):
    for 每个 destination:
        初始化 Q(current, next) = -edgeWeight
        重复多个 episode:
            选择一个起始节点
            重复最多 max steps:
                用 epsilon-greedy 选择动作
                reward = -edge weight
                Q = Q + alpha * (reward + gamma * maxFutureQ - Q)
                移动到 next

query(start, destination):
    如果 destination 没有 Q-table，则先训练
    current = start
    path = [start]
    重复最多 max steps:
        选择 Q 值最高的未访问邻居
        移动并累加边权
        如果到达终点，则返回路径
    return unreachable
```

### 路径构造

Q-learning 不像 Dijkstra 那样用前驱表重建路径。它是在查询过程中直接一步一步选择下一节点，并同步构造 `path`。

### 复杂度

设 D 为训练目标数量，K 为每个目标训练 episode 数，S 为每个 episode 最大步数，deg 为平均出度。

- 训练时间：约 O(DKS deg)。
- 查询时间：O(S deg)。
- 空间复杂度：O(DE)，因为每个目标都保存边上的 Q 值。

### 局限

Q-learning 在这里是实验性方法。它训练后查询很快，但没有数学上的最短路径保证。最新四个用例中它与 Dijkstra 成本相同，但这只是实验结果，不是理论证明。

## 16. ZeroHeuristic

### 算法说明

`ZeroHeuristic` 永远返回 0。如果 A\* 使用它，则优先级变成：

```text
f(n) = g(n) + 0
```

这就和 Dijkstra 的优先级规则一致。因此 A\* + ZeroHeuristic 的行为等价于 Dijkstra。

### 复杂度

- 单次估计时间：O(1)。
- 空间复杂度：O(1)。
- 搭配 A\* 时整体复杂度与堆优化 Dijkstra 相同。

### 用途

它是一个安全 fallback，也适合做正确性检查。如果某个复杂 heuristic 可能有问题，可以先换成 ZeroHeuristic，恢复 Dijkstra-like 行为。

## 17. 精确、近似和实验性算法总结

| Solver                          | 在当前加权图上是否保证最短路 | 主要用途                    |
| ------------------------------- | ---------------------------- | --------------------------- |
| DijkstraSolver                  | 是                           | 正式主算法                  |
| LinearDijkstraSolver            | 是                           | 无堆 baseline               |
| BidirectionalDijkstraSolver     | 是                           | 双向精确搜索                |
| BellmanFordSolver               | 是                           | 可处理负权边的替代算法      |
| SpfaSolver                      | 是                           | 队列优化 Bellman-Ford       |
| AStarSolver + LandmarkHeuristic | 是                           | 使用安全启发式加速          |
| AStarSolver + LearnedHeuristic  | 理论上不保证                 | 实验性学习启发式            |
| UnweightedBreadthFirstSolver    | 否，除非所有边权相同         | 无权图 / hop-count baseline |
| GreedyBestFirstSolver           | 否                           | 快速启发式近似              |
| QLearningRouteSolver            | 理论上不保证                 | 强化学习实验                |

## 18. 报告问题回答

### 18.1 程序使用了什么图算法？为什么适合本课程加权图？

正式程序使用堆优化 Dijkstra。它适合本课程图，因为基础设施网络是非负权加权图。Dijkstra 的理论前提正是边权非负，在这个条件下它能找到精确最短路径。

项目使用邻接表存图，也适合 Dijkstra。因为 Dijkstra 每次扩展一个节点时，只需要查看这个节点的出边。`PriorityQueue` 版本比线性扫描更高效，因为它能快速取出当前距离最小的待处理节点。

### 18.2 选定算法如何实现？时间和空间复杂度是多少？

堆优化 Dijkstra 主要使用三个结构：

- `distances`：起点到每个节点的当前最短已知距离；
- `previous`：用于重建路径的前驱节点；
- `PriorityQueue<NodeDistance>`：按距离排序的搜索前沿。

每当发现到某个邻居的更短路径时，程序更新距离和前驱，并把新的距离记录加入优先队列。如果之后从队列取出旧距离记录，程序会跳过它。

时间复杂度为 O((V + E) log V)。当前实现的工作空间为 O(V + E)，因为优先队列中可能存在重复旧条目。

### 18.3 每个 shortest-path query 都最优，是否代表整个巡检规划问题全局最优？

不是。每一段最短路径最优，不代表完整巡检规划问题全局最优。

课程用例中，途经点顺序是固定的。系统只是在这个固定顺序下，对每一段求最短路。这能正确回答题目给出的查询，但完整巡检规划还可能需要决定访问哪些地点、访问顺序、车辆或时间限制、是否允许重复经过节点等问题。这更接近旅行商问题或车辆路径规划，而不是单次最短路径查询。

因此，局部段最优是必要的，但不足以证明全局巡检计划最优。

### 18.4 如果图是无权图，可以考虑什么替代算法？

如果图是无权图，BFS 是最自然的选择。BFS 能在 O(V + E) 时间内找到边数最少的路径，比 Dijkstra 更简单。

如果所有边权都相同，BFS 的最少边数路径也就是最低总成本路径。但在当前加权图中，BFS 只优化 hop count，不保证总权重最小。

### 18.5 如果图更大，或有节点坐标，哪些方法可能更合适？

如果图规模显著增大，堆优化 Dijkstra 仍然正确，但可能需要探索太多节点。可以考虑：

- 使用坐标启发式的 A\*，例如欧氏距离。如果启发式不高估真实成本，可以减少搜索空间并保持最优性。
- ALT Landmark A\*。如果同一张图上有大量查询，预处理 landmark 距离后可以加速后续查询。
- 双向 Dijkstra 或双向 A\*。对于起终点较远的点对查询，可能减少搜索范围。
- Contraction Hierarchies 等道路网络预处理方法。它们查询极快，但实现复杂度更高。
- Q-learning 或 learned heuristic 适合实验探索，但需要训练，而且不能自动保证最优路线。

最终选择取决于系统更看重一次性实现简单、重复查询速度、内存开销，还是严格最优性保证。
