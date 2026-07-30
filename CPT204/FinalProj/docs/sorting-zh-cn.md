# Task A：排序报告中文详解版

## 1. 任务范围

Task A 的目标是对候选巡检地点进行排序，然后为后续路径规划选择优先级最高的地点。每一条候选记录包含两个字段：

- `location_id`：地点编号；
- `priority_score`：地点优先级分数。

本项目的排序规则有两层：

1. `priority_score` 越高，排序越靠前；
2. 如果两个地点的 `priority_score` 相同，则 `location_id` 更小的地点排在前面。

这个规则统一写在 `CandidateLocation.compareTo()` 里面。Bubble Sort、Quick Sort 和 Merge Sort 都调用同一个 `compareTo()`。这样做的好处是实验公平：三个算法的排序结果使用同一套标准，性能差异来自算法本身，而不是来自不同的比较规则。

在 Java 的 `Comparable` 规则里，`a.compareTo(b) < 0` 表示 `a` 应该排在 `b` 前面。本项目中，高分地点应该排在前面，所以分数更高的候选地点会被比较函数判定为“更小”，从而放到数组前部。

## 2. 数据集特点

三个候选文件都包含 1000 条记录，但初始顺序不同。初始顺序非常重要，因为不同排序算法对“已经接近有序”“随机无序”“大量重复值”的反应不同。

| 数据集      |               主要特点 | 相邻错序数 | 逆序对数 | 重复分数行数 |
| ----------- | ---------------------: | ---------: | -------: | -----------: |
| Candidate A |   基本已经符合最终排序 |         17 |       17 |            0 |
| Candidate B |   随机顺序，分数不重复 |        508 |   250696 |            0 |
| Candidate C | 大量重复分数，局部分组 |        485 |     5995 |         1000 |

“相邻错序数”表示相邻两条记录是否违反排序规则。例如低分在高分前面，或者分数相同但较大的 `location_id` 在较小的 `location_id` 前面，这就是相邻错序。

“逆序对数”范围更大。它统计任意两条记录之间是否顺序错误，即使这两条记录不相邻也会被计算。Bubble Sort 特别受逆序对影响，因为它只能通过相邻交换移动元素。

可以把三个数据集理解为三种典型输入：

- Candidate A：几乎已经排好，只需要修复少量局部错误。
- Candidate B：高度随机，很多元素距离最终位置很远。
- Candidate C：分数大量重复，因此 `location_id` 这个第二排序条件很重要。

这也解释了为什么三种基础算法没有一个能在所有数据集上都最快。

## 3. 实验方法

命令行实验会读取每个候选文件。每次运行排序算法之前，程序都会复制一份原始数组，然后对副本排序，并使用 `System.nanoTime()` 记录运行时间。每个算法在每个数据集上运行 20 次，输出 CSV 中保存每次 epoch 的时间和平均时间。

复制原始数组是必须的。否则第一个算法排序后，后面的算法拿到的就是已经排好序的数据，这会导致实验不公平。

本报告使用的最新 Task A 输出文件是：

- `output/taskA_candidate_a_epoch_times_20260518_220459.csv`
- `output/taskA_candidate_b_epoch_times_20260518_220459.csv`
- `output/taskA_candidate_c_epoch_times_20260518_220459.csv`

由于每个数据集只有 1000 行，Java 的短时间测量可能受到 JVM 预热、垃圾回收和系统负载影响。因此，实验数字需要结合算法复杂度和数据特点一起解释，不能只看单个时间值。

## 4. 必需算法一：Bubble Sort

### 4.1 Bubble Sort 如何工作

Bubble Sort 的核心思想是反复从左到右扫描数组。每次扫描时，它比较相邻两个元素。如果这两个元素顺序错误，就交换它们。

在本项目中，顺序是否错误由 `CandidateLocation.compareTo()` 判断。如果 `data[j].compareTo(data[j + 1]) > 0`，说明 `data[j]` 应该排在 `data[j + 1]` 后面，所以程序交换这两个元素。

经过一轮完整扫描后，一个元素会逐步移动到更接近最终位置的地方。由于本项目要把高优先级地点放在前面，高分地点会通过相邻交换逐渐向数组前部移动。

代码里还加入了 early-stop 优化。每一轮扫描都会记录是否发生过交换。如果某一轮没有任何交换，说明数组已经有序，算法可以立即停止。

### 4.2 为什么初始顺序会影响 Bubble Sort

Bubble Sort 对初始顺序非常敏感。原因是它只能交换相邻元素。如果一个应该排在前面的元素一开始在很靠后的位置，它必须一步一步向前移动，中间需要很多次交换。

Candidate A 只有 17 个逆序对，说明它基本已经有序。Bubble Sort 只需要修复少量局部错误，然后 early-stop 会提前结束，所以它在 Candidate A 上表现很好。

Candidate B 有 250696 个逆序对。很多元素距离最终位置很远，因此 Bubble Sort 需要大量相邻交换和多轮扫描，所以表现很差。

Candidate C 的总逆序对比 Candidate B 少，但它仍然有很多相邻错序，而且所有记录都属于重复分数组。算法除了比较分数，还经常需要比较 `location_id`。因此 Bubble Sort 在 Candidate C 上比 Candidate A 慢得多。

### 4.3 时间复杂度和空间复杂度

| 情况     | 时间复杂度 | 原因                                       |
| -------- | ---------: | ------------------------------------------ |
| 最好情况 |       O(n) | 数组已经有序，一轮扫描发现没有交换后停止。 |
| 平均情况 |     O(n^2) | 需要多轮扫描和大量相邻比较。               |
| 最坏情况 |     O(n^2) | 高度逆序或高度无序时，需要大量相邻交换。   |

空间复杂度是 O(1)。Bubble Sort 是原地排序，只需要少量变量，例如循环下标、交换用临时变量和 `swapped` 标记。

### 4.4 实验结果解释

Bubble Sort 在 Candidate A 上平均 151348 ns，是三种必需算法中最快的。这符合预期，因为 Candidate A 基本已经有序。

Bubble Sort 在 Candidate B 上平均 1965316 ns，是三种必需算法中最慢的。这也符合预期，因为随机数据有大量逆序对。

Bubble Sort 在 Candidate C 上平均 771445 ns。它比在 Candidate B 上快，但仍明显慢于 Merge Sort。原因是 Candidate C 虽然不是完全随机，但仍存在大量相邻错序和重复分数比较。

## 5. 必需算法二：Quick Sort

### 5.1 Quick Sort 如何工作

Quick Sort 使用分治思想。它先选择一个 pivot，然后把数组分成两部分：应该排在 pivot 前面的元素放左边，应该排在 pivot 后面的元素放右边。之后递归排序左右两部分。

本项目使用的是 Lomuto partition：

1. 选择当前区间最后一个元素作为 pivot；
2. 从左到右扫描当前区间；
3. 把所有应该排在 pivot 前面或与 pivot 相等的元素移动到左侧；
4. 把 pivot 放到左右两部分中间；
5. 递归排序左右两个子区间。

因此，当前实现确实是 Quick Sort，不是 Selection Sort。Selection Sort 会每次从剩余元素中找最小或最大元素，然后放到固定位置。本项目 Quick Sort 是围绕 pivot 分区，再递归处理子问题。

### 5.2 为什么 pivot 很重要

Quick Sort 快不快，关键取决于 pivot 能不能把数组分成比较平衡的两部分。如果 pivot 每次都接近中位数，左右子数组大小差不多，递归深度约为 log n，总时间复杂度就是 O(n log n)。

但如果 pivot 总是当前区间最小或最大元素，那么一边几乎为空，另一边几乎还是原来的大小。这样递归每次只减少一个元素，时间复杂度会退化到 O(n^2)。

本项目的基础 Quick Sort 总是选择最后一个元素作为 pivot。这个实现简单，但对已经有序或接近有序的数据很危险。

Candidate A 基本已经按目标规则排好。由于 pivot 总是取区间末尾元素，这个 pivot 很容易成为当前区间的极端值，导致分区非常不平衡。因此 Quick Sort 在 Candidate A 上接近最坏情况。

Candidate B 是随机顺序。区间最后一个元素不太可能一直是最小或最大值，所以分区通常更平衡。因此 Quick Sort 在 Candidate B 上非常快。

Candidate C 有大量重复分数。虽然 `location_id` 可以继续打破平局，保证排序正确，但重复值会让简单二路 partition 的效果变差，所以 Quick Sort 在 Candidate C 上没有明显优势。

### 5.3 时间复杂度和空间复杂度

| 情况     | 时间复杂度 | 原因                                         |
| -------- | ---------: | -------------------------------------------- |
| 最好情况 | O(n log n) | pivot 能把数组分成平衡的两部分。             |
| 平均情况 | O(n log n) | 随机数据通常不会一直选到极端 pivot。         |
| 最坏情况 |     O(n^2) | pivot 连续成为最小或最大元素，递归极不平衡。 |

空间复杂度平均为 O(log n)，来自递归调用栈。最坏情况下，如果分区一直极不平衡，递归深度可能达到 O(n)。当前实现不需要额外数组。

### 5.4 实验结果解释

Quick Sort 在 Candidate A 上平均 2543527 ns，是三种必需算法中最差的。这不是说明 Quick Sort 本身不好，而是说明“固定选择最后元素作为 pivot”的实现不适合接近有序的数据。

Quick Sort 在 Candidate B 上平均 52414 ns，是三种必需算法中最快的。随机顺序让 pivot 更可能产生相对平衡的分区。

Quick Sort 在 Candidate C 上平均 825148 ns，比 Bubble Sort 略慢，也明显慢于 Merge Sort。重复分数结构削弱了简单 partition 的优势。

## 6. 必需算法三：Merge Sort

### 6.1 Merge Sort 如何工作

Merge Sort 也使用分治思想，但它的划分方式比 Quick Sort 更稳定。它不靠 pivot，而是每次把数组区间平均分成左右两半。

算法步骤如下：

1. 把当前区间分成左右两半；
2. 递归排序左半部分；
3. 递归排序右半部分；
4. 把两个已经有序的部分合并成一个有序区间。

本项目实现会创建一个临时 `CandidateLocation[]` 数组用于合并。合并时，程序比较左半部分当前元素和右半部分当前元素，谁应该排在前面，就先复制谁。

当两个元素比较结果完全相等时，代码会先选择左半部分的元素。这让 merge 操作在完全相等的情况下保持稳定。

### 6.2 为什么 Merge Sort 更稳定

Merge Sort 不依赖输入是否接近有序，也不依赖 pivot 是否选得好。它总是把数据分成两半，再逐层合并。因此，对于 1000 条记录，它的递归层数大约是 log2(1000)，每一层合并时都会处理全部元素。

这意味着 Candidate A、Candidate B 和 Candidate C 上的工作量比较接近。已经接近有序的数据不会让 Merge Sort 像 Bubble Sort 那样大幅加速，但随机数据也不会让 Merge Sort 像 Bubble Sort 那样明显变慢。

对于 Candidate C，Merge Sort 特别合适。因为 Candidate C 有大量重复分数，排序需要频繁使用 `location_id` 作为第二规则。Merge Sort 会稳定、持续地应用同一个 `compareTo()` 规则，不会因为重复值导致严重分区问题。

### 6.3 时间复杂度和空间复杂度

| 情况     | 时间复杂度 | 原因                               |
| -------- | ---------: | ---------------------------------- |
| 最好情况 | O(n log n) | 即使已经有序，也仍然要分割和合并。 |
| 平均情况 | O(n log n) | 共有 log n 层，每层处理 n 个元素。 |
| 最坏情况 | O(n log n) | 平均分割结构不依赖初始顺序。       |

空间复杂度是 O(n)，因为需要一个辅助数组进行合并。递归栈还需要 O(log n)，但主要空间开销是 O(n) 临时数组。

### 6.4 实验结果解释

Merge Sort 在 Candidate A 上平均 186795 ns。它比 Bubble Sort 慢，因为 Candidate A 几乎有序，Bubble Sort 可以 early stop，而 Merge Sort 仍然要完整分割和合并。

Merge Sort 在 Candidate B 上平均 973333 ns。它比 Quick Sort 慢，但比 Bubble Sort 快很多。

Merge Sort 在 Candidate C 上平均 286593 ns，是三种必需算法中最快的。这说明 Merge Sort 很适合重复值较多、需要稳定处理 tie-breaking 的输入。

## 7. 扩展算法：Intro Sort

Intro Sort 是项目中的扩展算法，不属于课程要求必须比较的三种算法，但它很有讨论价值，因为它修复了基础 Quick Sort 的主要问题。

Intro Sort 一开始像 Quick Sort 一样工作，但它会监控递归深度。如果递归太深，说明分区可能很不平衡，算法就切换到 Heap Sort。这样可以避免 Quick Sort 的 O(n^2) 最坏情况。对于很小的子区间，Intro Sort 会使用 Insertion Sort，因为小规模数组上插入排序的常数开销低。

本项目中的 Intro Sort 有几个关键点：

- 深度限制为 `2 * floorLog2(n)`；
- pivot 使用 median-of-three，而不是固定最后一个元素；
- 小区间最后用 Insertion Sort 处理；
- 超过深度限制时使用 Heap Sort fallback。

最新实验中，Intro Sort 在 Candidate A 上平均 277437 ns，在 Candidate B 上平均 296444 ns，在 Candidate C 上平均 371149 ns。它不是必需算法比较的核心结论，但能说明为什么很多生产环境排序库会使用 introspective sorting：它兼顾 Quick Sort 的平均速度和 Heap Sort 的最坏情况保障。

Intro Sort 最坏时间复杂度是 O(n log n)，因为有 Heap Sort fallback。额外空间通常是 O(log n) 递归栈，堆排序部分原地进行。

## 8. 扩展算法：Hybrid Adaptive Sort

Hybrid Adaptive Sort 是项目中的另一个扩展算法。它不是传统固定算法，而是先分析数据形态，再选择合适策略。

排序前，它会计算两个指标：

- `breakRatio`：相邻元素中错序的比例；
- `duplicateScoreRatio`：有重复分数的记录所占比例。

然后它按规则选择算法：

| 检测到的数据形态 | 代码条件                      | 使用策略                         |
| ---------------- | ----------------------------- | -------------------------------- |
| 接近有序         | `breakRatio <= 0.08`          | 用类似 Insertion Sort 的局部修复 |
| 大量重复分数     | `duplicateScoreRatio >= 0.20` | 使用 Merge Sort                  |
| 普通无序数据     | 其他情况                      | 使用 Intro Sort                  |

Candidate A 的 `breakRatio` 约为 0.017，所以被识别为接近有序，使用局部修复。Candidate B 是随机顺序且没有重复分数，所以使用 Intro Sort。Candidate C 所有记录都属于重复分数组，所以使用 Merge Sort。

这个设计的思路很直观：先判断数据属于哪一类，再选择适合这类数据的排序策略。但 profiling 本身也有开销。当前实现会扫描输入，并创建一个整数数组统计分数重复情况。对于 1000 行这样的小数据集，额外 profiling 成本可能比较明显。

最新实验中，Hybrid Adaptive Sort 在 Candidate A 上平均 350141 ns，在 Candidate B 上平均 291598 ns，在 Candidate C 上平均 262200 ns。它在 Candidate C 上表现很好，因为它正确识别了大量重复分数并选择 Merge Sort。它在 Candidate A 上没有 Bubble Sort 快，因为数据规模小且已经接近有序，Bubble Sort 的 early stop 已经足够有效，而 Hybrid 还要支付 profiling 成本。

Hybrid Adaptive Sort 的时间复杂度取决于选择的分支。局部修复在接近有序时很快，但如果大量元素都要移动，可能接近 O(n^2)。Merge Sort 分支是 O(n log n)，Intro Sort 分支最坏也是 O(n log n)。空间复杂度在当前实现中是 O(n)，因为 profiling 会创建分数数组，Merge Sort 分支也需要 O(n) 辅助数组。

## 9. 三种必需算法的实验结果

| 数据集      | Bubble Sort 平均 ns | Quick Sort 平均 ns | Merge Sort 平均 ns | 三种必需算法中最快 |
| ----------- | ------------------: | -----------------: | -----------------: | ------------------ |
| Candidate A |              151348 |            2543527 |             186795 | Bubble Sort        |
| Candidate B |             1965316 |              52414 |             973333 | Quick Sort         |
| Candidate C |              771445 |             825148 |             286593 | Merge Sort         |

结果和数据特点一致：

- Candidate A 基本有序，所以 Bubble Sort 受益最大。
- Candidate B 随机无序，所以 Quick Sort 的平均情况优势最大。
- Candidate C 大量重复分数，所以 Merge Sort 的稳定处理更适合。

这说明算法选择不能只看一个数据集。不同算法适合不同输入形态。

## 10. 每个数据集选出的 Top 10 地点

排序完成后，系统会从每个数据集中取前 10 条记录。这些记录就是按照要求排序规则选出的最高优先级地点。

| 数据集      | 选出的 Top 10 地点                                                   |
| ----------- | -------------------------------------------------------------------- |
| Candidate A | L0001, L0002, L0003, L0004, L0005, L0006, L0007, L0008, L0009, L0010 |
| Candidate B | L0101, L0102, L0103, L0104, L0105, L0106, L0107, L0108, L0109, L0110 |
| Candidate C | L0201, L0202, L0203, L0204, L0205, L0206, L0207, L0208, L0209, L0210 |

这个结果也说明第二排序条件很重要。Candidate C 的 Top 10 地点分数都相同，都是 5000。因此它们之间的顺序不是由分数决定，而是由 `location_id` 升序决定。

这 30 个地点会作为后续路径规划阶段的候选巡检目标。因此，排序阶段不仅影响算法运行时间，也会影响完整巡检流程中的优先级选择。

## 11. 报告问题回答

### 11.1 初始顺序如何影响 Bubble Sort、Quick Sort 和 Merge Sort？

初始顺序对 Bubble Sort 影响很大。如果输入已经接近有序，Bubble Sort 只需要少量相邻交换，并且可以通过 early stop 提前结束，所以 Candidate A 上很快。如果输入随机无序，很多元素离最终位置很远，Bubble Sort 只能一步一步相邻交换，所以 Candidate B 上很慢。

初始顺序对当前 Quick Sort 实现也影响很大，但原因不同。Quick Sort 依赖 pivot 的质量。本项目基础 Quick Sort 总是选择最后一个元素作为 pivot，所以接近有序的数据容易产生极不平衡分区。Candidate A 因此导致接近最坏情况。Candidate B 是随机顺序，pivot 通常不会一直是极端值，所以效果很好。

Merge Sort 受初始顺序影响最小。它总是分半、排序、合并。无论输入接近有序、随机无序还是大量重复，它的主要工作量都接近 O(n log n)。

### 11.2 三种算法在每个数据集上哪个最好？

Candidate A 上 Bubble Sort 最好。原因是 Candidate A 只有 17 个逆序对，已经非常接近最终顺序。Bubble Sort 的 early stop 正好适合这种输入。

Candidate B 上 Quick Sort 最好。原因是 Candidate B 的随机顺序让固定末尾 pivot 通常能产生较平衡的分区，从而发挥 Quick Sort 平均 O(n log n) 的优势。

Candidate C 上 Merge Sort 最好。原因是 Candidate C 有大量重复分数，需要稳定、可预测地处理 tie-breaking。Merge Sort 不会像基础 Quick Sort 那样受到重复值 partition 效果的明显影响。

### 11.3 哪个算法在三个数据集上最稳定？为什么？

三种必需算法中，Merge Sort 最稳定。它的性能来自固定的分治合并结构，不依赖逆序对数量，也不依赖 pivot 是否选得好。它不会因为输入接近有序而特别快，但也不会因为随机或重复值而严重退化。

Bubble Sort 不稳定，因为它在接近有序数据上很好，在随机数据上很差。Quick Sort 在本项目中也不稳定，因为固定末尾 pivot 在 Candidate B 上很快，但在 Candidate A 上很差。

### 11.4 如果最终系统只能使用一种排序算法，应优先考虑什么？

最终系统应优先考虑稳定性，而不是只追求某一个数据集上的最快平均时间。真实系统未来可能收到不同顺序模式的数据。如果算法只在一种输入上很快，但在另一种输入上严重退化，系统可靠性会下降。

在三种必需算法中，推荐 Merge Sort 作为最终系统的保守选择。它在所有情况下都有 O(n log n) 时间复杂度，并且能清晰处理重复分数和 `location_id` tie-breaking。它的缺点是需要 O(n) 额外空间，但对于候选地点排序来说，通常可以接受。

如果允许使用扩展算法，Intro Sort 是更强的工程选择，因为它兼顾 Quick Sort 的平均速度和 Heap Sort 的最坏情况保护。

### 11.5 如果候选地点数量显著增加，哪个算法最合适？

如果数据量显著增加，不应使用 Bubble Sort。它的 O(n^2) 增长太快，在随机或高度无序数据上会变得不现实。

基础 Quick Sort 平均 O(n log n)，内存开销也较小，但当前实现固定选择末尾元素作为 pivot。对于大规模接近有序输入，它可能退化到 O(n^2)，风险较高。

Merge Sort 适合大规模数据，前提是系统有足够内存。它的 O(n log n) 时间稳定，但需要 O(n) 额外空间。

如果可以使用项目中的扩展算法，Intro Sort 最适合大规模场景。它有 Quick Sort 的实际速度，额外空间较低，并通过 Heap Sort fallback 避免最坏情况。

### 11.6 如果同时考虑运行时间和内存使用，选择会如何变化？

考虑内存后，选择会更平衡。Merge Sort 时间稳定，但需要 O(n) 辅助数组。如果数据量达到几百万，并且系统内存有限，这个额外空间可能成为问题。

Bubble Sort 内存最好，是 O(1)，但运行时间在随机或大规模输入上太差。低内存不能弥补严重的时间问题。

Quick Sort 平均时间好，平均额外空间也较低，但当前 pivot 策略存在最坏情况风险。生产版本应改进 pivot 策略，或使用随机 pivot、三路 partition、Intro Sort 等方式。

因此：

- 如果最看重稳定运行时间和重复值处理，选择 Merge Sort；
- 如果同时看重运行时间和内存效率，选择 Intro Sort；
- Bubble Sort 只适合小规模或几乎有序的数据；
- 当前基础 Quick Sort 不适合作为唯一最终算法，除非改进 pivot 策略。

## 12. 最终建议

对于课程要求的三种算法比较，最佳算法取决于数据集：Candidate A 选 Bubble Sort，Candidate B 选 Quick Sort，Candidate C 选 Merge Sort。

对于最终系统，稳定性比在单个数据集上最快更重要。在三种必需算法中，推荐 Merge Sort，因为它有稳定的 O(n log n) 时间复杂度，并且能可靠处理重复分数。如果允许使用扩展实现，Intro Sort 是更好的工程选择，因为它同时具备较好的平均速度、较低的额外空间和最坏情况保护。
