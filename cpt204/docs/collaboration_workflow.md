# 协作工作流

## 1. 推荐分支

```text
main
├── feature/task-a-sorting
├── feature/task-b-graph
├── feature/report-chapter-1-2
├── feature/report-chapter-3-4
└── feature/presentation
```

## 2. 基本 Git 工作流

```bash
git switch -c feature/task-a-sorting
# 编辑文件
git add .
git commit -m "feat: 实现候选排序规则"
git push -u origin feature/task-a-sorting
```

然后在 GitHub 上发起 Pull Request。

## 3. 审查清单

合并 PR 之前：

- 项目能否编译？
- 分支是否遵循所需的排序规则或图规则？
- 代码是否足够易懂，能在面试中解释？
- 重要算法步骤是否有注释？
- 输出文件是否放在 `output/` 中，而非随意放在根目录？
- 报告笔记是否放在 `docs/` 或 `report/` 中？

## 4. 建议团队角色

### 学生 1

- 候选 CSV 读取器
- 候选数据模型
- 冒泡排序、快速排序、归并排序
- 排序计时实验
- 第 1 章报告笔记

### 学生 2

- 图 CSV 读取器
- 图与边的数据模型
- Dijkstra 最短路径求解器
- 路线查询服务
- 第 2 章报告笔记

### 两人共同

- 第 3 章面向对象设计解释
- 第 4 章反思
- UML 图
- PPT 幻灯片
- 视频脚本与录制

## 5. 会议记录格式

每次会议按如下格式创建一个小节：

```markdown
## 会议 YYYY-MM-DD

参与人员：
- 学生 A
- 学生 B

讨论内容：
- ...

决定：
- ...

后续行动：
- [ ] ...
```

## 6. 避免冲突

- 同一报告段落不应两人同时编辑。
- 使用小规模的 PR。
- 在更改共享接口（如 `Sorter` 或 `ShortestPathSolver`）之前进行沟通。
