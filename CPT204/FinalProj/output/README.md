# 输出文件夹

程序会在此文件夹生成实验日志。不要手动编辑这些 CSV，除非明确要重新整理实验数据。

```text
taskA_candidate_*_epoch_times_*.csv
taskB_route_results_*.csv
taskB_algorithm_epoch_times_*.csv
experiment_summary_*.csv
```

文件含义：

- `taskA_candidate_*_epoch_times_*.csv`：Task A 排序实验，每个数据集、每个算法 20 epochs，包含单次耗时和平均耗时。
- `taskB_route_results_*.csv`：Task B 四个正式路径查询的路径和总成本。
- `taskB_algorithm_epoch_times_*.csv`：Task B 多算法实验日志，每个算法 20 epochs，包含预处理耗时和成本一致性校验。
- `experiment_summary_*.csv`：从最新实验日志直接计算出的汇总表。
