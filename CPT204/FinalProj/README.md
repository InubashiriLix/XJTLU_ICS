# 城市基础设施巡检系统

CPT204 面向对象 Java 应用的小组项目模板。(实际上已经是成品了, 不过懒得改了)

本仓库**刻意留空但可编译**(并非)。它包含一个干净的 Java 骨架代码、文档文件、协作记录和输出文件夹。真实的 CSV 数据集和最终的报告/视频文件应由团队后续添加。

## 1. 项目目标

课程作业要求团队构建一个城市基础设施巡检系统。预期工作流程如下：

1. 读取三个候选数据集：`candidates_A.csv`、`candidates_B.csv` 和 `candidates_C.csv`。
2. 按照要求的排序规则对每个数据集进行排序：
   - `priority_score` 降序排列；
   - 分数相同时，`location_id` 升序排列。
3. 比较冒泡排序、快速排序和归并排序。
4. 从每个数据集中选取前 10 个地点，共得到 30 个选定的巡检目标。
5. 读取 `paths.csv` 作为无向加权图。
6. 使用最短路径算法回答所需的 Task B 路线查询。
7. 阐述完整应用设计、数据结构、面向对象原则、团队协作过程、AI 使用情况、EDI（平等、多样性、包容性）反思及未来改进。

## 2. 仓库结构

```text
urban-infrastructure-inspection-template/
├── README.md
├── .gitignore
├── data/
│   └── .gitkeep
├── src/
│   └── cpt204/
│       └── inspection/
│           ├── Main.java
│           ├── model/
│           ├── io/
│           ├── sorting/
│           ├── graph/
│           └── util/
├── output/
│   └── screenshots/
├── report/
│   └── figures/
├── presentation/
├── scripts/
│   ├── compile.sh
│   ├── run.sh
│   └── clean.sh
├── docs/
│   ├── architecture.md
│   ├── coding_standard.md
│   ├── collaboration_workflow.md
│   ├── report_mapping.md
│   ├── task_allocation.md
│   ├── ai_usage_log.md
│   └── project_checklist.md
└── .github/
    └── pull_request_template.md
```

## 3. 环境要求

本模板不使用 Maven、Gradle 或任何第三方库，仅需 JDK。

推荐：

```bash
java -version
javac -version
```

任意正常的较新版本 JDK 均可运行。项目仅使用 Java 标准库类，例如 `ArrayList`、`HashMap`、`PriorityQueue`、`BufferedReader` 和 `FileReader`。

## 4. 如何编译

在项目根目录下：

```bash
bash scripts/compile.sh
```

预期输出：

```text
Compile successful. Class files are in ./out
```

手动等价命令：

```bash
mkdir -p out
javac -encoding UTF-8 -d out $(find src -name "*.java")
```

## 5. 如何运行

```bash
bash scripts/run.sh
```

预期输出：

```text
Urban Infrastructure Inspection System - template
This skeleton compiles and is ready for Task A, B, C, and D implementation.
Loaded components:
- CandidateCsvReader
- GraphCsvReader
- SortingExperiment
- RouteQueryService
Next step: put candidates_A.csv, candidates_B.csv, candidates_C.csv, and paths.csv into data/.
```

## 5.1 如何运行 JavaFX GUI

GUI 是一个独立入口，不替换命令行实验入口。命令行版本继续使用 `scripts/run.sh`，JavaFX 原型使用：

```bash
bash scripts/compile-gui.sh
bash scripts/run-gui.sh
```

默认脚本使用 `/opt/stm32cubeclt/jre` 中带 JavaFX 模块的 JDK。如果本机 JavaFX JDK 在其他路径，可以这样指定：

```bash
JAVAFX_JDK=/path/to/javafx-jdk bash scripts/run-gui.sh
```

GUI 提供两个可视化页面：

- Task A Visual Sorting：选择候选数据集和排序算法，通过柱状图动画展示比较、交换、分区或归并过程。
- Task B Route Map：在基础设施图上显示节点、边、起点、途经点、终点和高亮路径，可切换全部 Task B 路径算法，包括线性 Dijkstra、堆优化 Dijkstra、双向 Dijkstra、SPFA、Bellman-Ford、BFS 近似、A* Landmark、Greedy Best-First、学习型启发式和 Q-learning Routing。

命令行入口仍负责正式 20-epoch 实验和 CSV 日志。GUI 入口主要用于课堂演示和算法过程解释。

JavaFX 需要图形桌面环境。如果终端没有 `DISPLAY`，GUI 会无法启动，但普通命令行实验仍可运行。

## 6. 课程作业数据放置位置

将真实的课程作业 CSV 文件放入 `data/` 目录：

```text
data/
├── candidates_A.csv
├── candidates_B.csv
├── candidates_C.csv
└── paths.csv
```

模板当前不包含实际的 CSV 文件。

候选文件预期列：

```csv
location_id,priority_score
L0001,98
L0002,75
```

路径文件预期列：

```csv
from_location,to_location,weight
L0001,L0002,5.5
L0002,L0003,3.0
```

## 7. 当前 Java 包设计

### `model/`

包含核心数据对象：

- `CandidateLocation`：一个包含 `locationId` 和 `priorityScore` 的候选地点。
- `Edge`：一条带权图边。
- `Graph`：以邻接表存储的无向加权图。
- `PathResult`：最短路径结果，包含路径、总成本和可达性。

### `io/`

包含 CSV 输入类：

- `CandidateCsvReader`：读取候选文件。
- `GraphCsvReader`：读取 `paths.csv` 并构建图。

### `sorting/`

包含 Task A 排序逻辑：

- `Sorter`：排序算法的通用接口。
- `BubbleSorter`：冒泡排序实现。
- `QuickSorter`：快速排序实现。
- `MergeSorter`：归并排序实现。
- `SortingExperiment`：执行计时实验并收集前 10 名结果。

### `graph/`

包含 Task B 路线逻辑：

- `ShortestPathSolver`：最短路径算法的通用接口。
- `DijkstraSolver`：用于非负权加权图的 Dijkstra 实现。
- `RouteQueryService`：处理直达和途经点路径查询。

### `util/`

包含共享辅助类：

- `Timer`：封装 `System.nanoTime()` 用于计时实验。
- `ResultPrinter`：打印适用于报告的格式化输出。

## 8. 推荐的 GitHub 协作流程

使用分支而非直接推送到 `main`。

建议分支：

```text
main
├── feature/task-a-sorting
├── feature/task-b-graph
├── feature/report-chapter-1-2
├── feature/report-chapter-3-4
└── feature/presentation
```

推荐流程：

1. 创建新分支。
2. 进行小步提交。
3. 推送分支。
4. 发起 Pull Request。
5. 队友审查代码。
6. 编译通过后再合并到 `main`。

## 9. 建议的提交信息风格

```text
feat: 添加候选 CSV 读取器
feat: 实现冒泡排序
feat: 实现 Dijkstra 求解器
docs: 添加 Task A 报告笔记
fix: 修正候选排序规则
refactor: 将路径查询逻辑移入 RouteQueryService
```

## 10. 与课程报告各章对应关系

本项目的结构设计旨在使最终报告更易撰写：

| 报告章节           | 相关项目文件                                                                        |
| ------------------ | ----------------------------------------------------------------------------------- |
| 第 1 章 - 排序算法 | `sorting/`、`model/CandidateLocation.java`、`output/taskA_sorting_results.csv`      |
| 第 2 章 - 图算法   | `graph/`、`model/Graph.java`、`model/Edge.java`、`output/taskB_path_results.txt`    |
| 第 3 章 - 应用设计 | `model/`、`io/`、`sorting/`、`graph/`、`docs/architecture.md`                       |
| 第 4 章 - 项目反思 | `docs/task_allocation.md`、`docs/ai_usage_log.md`、`docs/collaboration_workflow.md` |
| 第 5 章 - 程序代码 | `src/` 下所有文件                                                                   |
| 第 6 章 - 附录     | `output/`、`report/figures/`、截图                                                  |
| 第 7 章 - 贡献表   | `docs/task_allocation.md`                                                           |

## 11. 重要的实现注意事项

- 不要将所有逻辑隐藏在 `Main.java` 中。
- 每个类专注于单一职责。
- 所有排序算法使用相同的排序规则。
- 计时不同排序算法时使用原始数据集的副本。
- 如需平均运行时间，对每个排序算法运行多次。
- 对于 Task B，30 个选定地点是完整图中的重要节点，而非图中的唯一节点。
- 对于途经点查询，按顺序计算每个所需分段，并仔细合并路径。

## 12. 学术诚信提醒

在 `docs/ai_usage_log.md` 中记录 AI 使用情况。最终报告应仅引用根据模块政策允许的 AI 使用。不要提交无法当面解释的代码。
