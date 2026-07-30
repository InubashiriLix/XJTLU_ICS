# 报告映射指南

本文档说明仓库文件如何对应到最终的 Word 报告。

## 第 1 章 - 排序算法，Task A

相关文件：

```text
src/cpt204/inspection/model/CandidateLocation.java
src/cpt204/inspection/sorting/Sorter.java
src/cpt204/inspection/sorting/BubbleSorter.java
src/cpt204/inspection/sorting/QuickSorter.java
src/cpt204/inspection/sorting/MergeSorter.java
src/cpt204/inspection/sorting/SortingExperiment.java
```

报告应包含：

- 排序规则。
- 为什么所有算法使用相同的 `compareTo()` 规则。
- 三个数据集的 20-epoch 计时表格。
- 每个数据集的前 10 个地点。
- 有序/无序输入效果分析。
- 哪个算法最快，哪个最稳定。
- 可扩展性讨论。
- 运行时间与内存讨论。

可使用的输出文件：

```text
output/taskA_candidate_a_epoch_times_20260515_213804.csv
output/taskA_candidate_b_epoch_times_20260515_213804.csv
output/taskA_candidate_c_epoch_times_20260515_213804.csv
output/experiment_summary_20260515_213806.csv
```

## 第 2 章 - 图算法，Task B

相关文件：

```text
src/cpt204/inspection/model/Graph.java
src/cpt204/inspection/model/Edge.java
src/cpt204/inspection/model/PathResult.java
src/cpt204/inspection/io/GraphCsvReader.java
src/cpt204/inspection/graph/ShortestPathSolver.java
src/cpt204/inspection/graph/DijkstraSolver.java
src/cpt204/inspection/graph/RouteQueryService.java
src/cpt204/inspection/graph/AStarSolver.java
src/cpt204/inspection/graph/LandmarkHeuristic.java
src/cpt204/inspection/graph/LearnedHeuristic.java
src/cpt204/inspection/graph/QLearningRouteSolver.java
src-javafx/cpt204/inspection/gui/GuiMain.java
```

报告应包含四个用例：

1. A1 到 A1。
2. A1 到 A10。
3. A1 到 B1，途径 B5。
4. A1 到 C1，途径 B5 和 C5，按此顺序。

每个用例包含：

- 起始节点；
- 目标节点；
- 途经点（如有）；
- 最短路径；
- 总成本。

分析应解释：

- 为什么 Dijkstra 适用于加权图；
- 堆优化 Dijkstra 与线性 Dijkstra 的差异；
- A* 地标启发式的预处理成本和查询速度；
- Q-learning 路由为什么是实验性方法；
- 为什么 BFS 和 Greedy 可以很快但不保证最短成本；
- 时间复杂度；
- 空间复杂度；
- 为什么局部最短路径查询不一定能全局最优地解决巡检规划问题；
- 替代方案，如 Bellman-Ford、SPFA、双向 Dijkstra、A*、Q-learning。

可使用的输出文件：

```text
output/taskB_route_results_20260515_213804.csv
output/taskB_algorithm_epoch_times_20260515_213806.csv
output/experiment_summary_20260515_213806.csv
```

## 第 3 章 - 整体应用设计，Task C

相关文件：

```text
docs/architecture.md
src/cpt204/inspection/model/
src/cpt204/inspection/io/
src/cpt204/inspection/sorting/
src/cpt204/inspection/graph/
src/cpt204/inspection/util/
src-javafx/cpt204/inspection/gui/
```

报告应包含：

- 数据结构；
- 类的职责；
- 公有/私有函数；
- 面向对象原则；
- 命令行实验入口和 JavaFX GUI 入口的分离；
- GUI 如何将排序过程和路径搜索结果可视化；
- UML 或包图。

## 第 4 章 - 项目反思，Task D

相关文件：

```text
docs/task_allocation.md
docs/ai_usage_log.md
docs/collaboration_workflow.md
```

报告应包含：

- AI 辅助规划与协作；
- AI 赋能项目管理的优缺点；
- 平等、多样性与包容性（EDI）讨论；
- 未来优化；
- 终身学习反思；
- 个人/团队贡献。

## 第 5 章 - 程序代码

将所有 `src/` 下的 Java 源文件以文本形式复制到报告中。第 5 章不要使用截图。

## 第 6 章 - 附录

可能的附录材料：

- 控制台输出；
- 20-epoch 计时结果；
- 最短路径输出；
- 多算法实验汇总 CSV；
- UML 图；
- AI 使用引用；
- 证明程序可运行的截图。

## 第 7 章 - 贡献表

使用 `docs/task_allocation.md` 填写最终贡献表。
