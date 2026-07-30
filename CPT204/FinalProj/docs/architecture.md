# 架构设计笔记

## 1. 设计目标

应用不应是一个单文件的过程式程序。课程作业要求面向对象设计、算法实现以及数据结构的解释。因此，代码被清晰地划分为不同的包：

```text
model     数据对象与图的表示
io        CSV 读取
sorting   Task A 排序算法与实验执行器
graph     Task B 最短路径算法与路线查询处理
util      共享辅助类
src-javafx JavaFX 交互式原型界面
```

## 2. 高层工作流

```text
Main
 ├── CandidateCsvReader 读取 candidates_A/B/C.csv
 ├── SortingExperiment 运行冒泡、快速和归并排序，每个算法 20 epochs
 ├── 从每个数据集选取前 10 名
 ├── GraphCsvReader 读取 paths.csv
 ├── RouteQueryService 执行 Task B 直达和途经点查询
 ├── DijkstraSolver 生成正式最短路径结果
 └── TaskBExperimentRunner 对多种图算法和学习型策略进行 20-epoch 对照实验

GuiMain
 ├── Task A Visual Sorting tab 用柱状图动画展示排序过程
 └── Task B Route Map tab 绘制图节点、边和高亮路径，并可切换全部 Task B 路径算法
```

## 3. 关键类

### CandidateLocation

表示候选 CSV 文件中的一行。

字段：

- `locationId`
- `priorityScore`

它实现了 `Comparable<CandidateLocation>`，使所有排序算法可以使用相同的排序规则。

### Sorter

所有排序算法使用的接口。

```java
public interface Sorter {
    void sort(CandidateLocation[] data);
    String getName();
}
```

这支持多态：`SortingExperiment` 可以通过同一个接口运行任意排序器。

### Graph

以邻接表存储基础设施网络：

```text
Map<String, List<Edge>> adjacencyList
```

这适用于图遍历和 Dijkstra 算法，因为可以快速找到从某个节点出发的所有出边。

### ShortestPathSolver

最短路径算法的接口。

```java
public interface ShortestPathSolver {
    PathResult findShortestPath(Graph graph, String startLocation, String endLocation);
}
```

这使得程序可以替换不同路径求解器。目前已实现的求解器包括：

- `DijkstraSolver`：使用 `PriorityQueue` 的堆优化 Dijkstra。
- `LinearDijkstraSolver`：不使用堆的线性扫描 Dijkstra，用于对照堆优化效果。
- `BidirectionalDijkstraSolver`：双向 Dijkstra。
- `AStarSolver`：A* 搜索，可搭配不同启发式。
- `BellmanFordSolver`：Bellman-Ford。
- `SpfaSolver`：队列优化 Bellman-Ford 变体。
- `UnweightedBreadthFirstSolver`：忽略权重的 BFS，作为近似策略对照。
- `GreedyBestFirstSolver`：贪心最佳优先搜索，作为近似策略对照。
- `QLearningRouteSolver`：纯 Java 表格型 Q-learning 路由实验。

启发式实现包括：

- `ZeroHeuristic`：零启发式，A* 会退化为 Dijkstra 行为。
- `LandmarkHeuristic`：ALT 风格地标启发式，使用三角不等式下界。
- `LearnedHeuristic`：纯 Java 线性回归启发式，属于实验性学习方法。

### RouteQueryService

处理直达和途经点查询。这可以避免 `Main.java` 过于庞大。

### GuiMain

JavaFX GUI 入口，位于 `src-javafx/cpt204/inspection/gui/GuiMain.java`。它复用现有读取器、排序规则、路径求解器和图结构，只负责交互可视化，不复制核心算法。GUI 与命令行入口分离：

- `scripts/run.sh`：运行完整命令行实验。
- `scripts/run-gui.sh`：打开 JavaFX 教育型可视化原型。

GUI 的定位不是替代实验日志，而是帮助没有深入算法背景的用户理解过程：

- Task A 使用柱状图逐帧展示比较、交换、分区和归并。
- Task B 使用路线聚焦子图展示基础设施网络，并高亮当前路线、起点、途经点和终点。算法下拉框覆盖命令行实验中的全部路径算法：线性 Dijkstra、堆优化 Dijkstra、双向 Dijkstra、SPFA、Bellman-Ford、BFS 近似、A* Landmark、Greedy Best-First、学习型启发式和 Q-learning Routing。

## 4. 使用的面向对象原则

### 封装

字段是私有的，对象暴露公有的 getter 或方法。

示例：

- `CandidateLocation.locationId` 是私有的。
- `PathResult.path` 在返回前进行复制。
- `Graph.adjacencyList` 不直接暴露。

### 抽象

接口隐藏实现细节：

- `Sorter`
- `ShortestPathSolver`

程序其余部分不需要知道冒泡排序、快速排序、归并排序或 Dijkstra 的内部实现。

### 多态

程序可以将不同的排序算法存储在同一数组中：

```java
Sorter[] sorters = {
    new BubbleSorter(),
    new QuickSorter(),
    new MergeSorter()
};
```

然后 `SortingExperiment` 可以调用 `sorter.sort(data)` 而无需检查具体类。

### 单一职责

每个类有明确的职责：

- 读取器负责读取数据；
- 排序器负责排序数据；
- 图类负责表示网络；
- 求解器负责计算最短路径；
- 结果对象负责存储输出。

## 5. 数据结构

| 用途 | 数据结构 | 原因 |
|---|---|---|
| 候选行 | `CandidateLocation[]` / `ArrayList<CandidateLocation>` | 便于排序和实验复制 |
| 加权图 | `HashMap<String, List<Edge>>` | 高效的邻接查找 |
| Dijkstra 队列 | `PriorityQueue` | 高效提取下一个最近节点 |
| Bellman-Ford 边集 | `ArrayList<Edge>` | 对所有边重复松弛 |
| SPFA 队列 | `ArrayDeque<String>` | 只重新处理被更新影响的节点 |
| A* 开放集 | `PriorityQueue` | 按 `g(n) + h(n)` 优先扩展 |
| Q-learning 表 | `HashMap<String, Map<String, Map<String, Double>>>` | 按目标点、当前节点和下一跳存储 Q 值 |
| 距离 | `HashMap<String, Double>` | 按地点 ID 快速查找距离 |
| 前驱节点 | `HashMap<String, String>` | 重构最短路径 |

## 6. 输出与实验记录

当前程序会生成以下类型的输出：

- `output/taskA_candidate_*_epoch_times_*.csv`：Task A 每个数据集、每个排序算法的 20 次 epoch 耗时和平均耗时。
- `output/taskB_route_results_*.csv`：Task B 四个正式路径查询结果。
- `output/taskB_algorithm_epoch_times_*.csv`：Task B 多算法路径实验，每个算法 20 epochs。
- `output/experiment_summary_*.csv`：由最新日志直接计算出的汇总表。

近似算法和学习型算法会通过 `same_route_costs` 字段与 Dijkstra 基准结果比较。`true` 表示该次实验中总成本一致，`false` 表示策略速度可能更快但没有得到相同最短路径成本。
