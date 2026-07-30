# 编码规范

## 1. 语言与库规则

- 仅使用 Java。
- 优先使用课程或教材中涵盖的标准库类。
- 除非模块负责人明确允许，否则避免使用第三方库。
- 保持项目可使用纯 `javac` 编译。

## 2. 命名规则

| 条目 | 风格 | 示例 |
|---|---|---|
| 类 | PascalCase（大驼峰） | `CandidateLocation` |
| 接口 | PascalCase（大驼峰） | `Sorter` |
| 方法 | camelCase（小驼峰） | `findShortestPath` |
| 变量 | camelCase（小驼峰） | `priorityScore` |
| 常量 | UPPER_SNAKE_CASE | `DEFAULT_REPEAT_TIMES` |
| 包 | 全小写 | `cpt204.inspection.sorting` |

## 3. 类设计规则

- 每个类承担一个主要职责。
- 避免将业务逻辑直接放在 `Main.java` 中。
- 除非有充分理由，否则字段应保持私有。
- 当多个实现共享相同行为时使用接口。
- 对算法和非显而易见的逻辑添加注释。

## 4. 排序规则

每种排序算法必须使用以下确切的排序规则：

1. 按 `priority_score` 降序排列。
2. 如果两行得分相同，按 `location_id` 升序排列。

该规则在 `CandidateLocation.compareTo()` 中实现，因此冒泡排序、快速排序和归并排序共享相同的逻辑。

## 5. 计时规则

- 通过 `Timer` 辅助类使用 `System.nanoTime()`。
- 在每次算法运行之前复制原始数据集。
- 除非是有意实验，否则不要对已排序的数组再次排序。
- 考虑对每个算法运行 3 次并报告平均运行时间。

## 6. 图规则

- 将 `paths.csv` 视为无向加权图。
- 为每条边添加两个方向的连接。
- 仅在权重非负时使用 Dijkstra。
- 将路径重构与图构建分开。

## 7. Pull Request 规则

发起 PR 之前：

- 运行 `bash scripts/compile.sh`。
- 运行 `bash scripts/run.sh`。
- 确保没有 `.class`、`out/` 或大型 MP4 文件被提交。
- 说明变更内容和测试方式。
