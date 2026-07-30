# Task A: Sorting Report

## 1. Task Scope

Task A asks the system to sort candidate inspection locations and select the top locations for later route planning. Each candidate record contains:

- `location_id`: the identifier of the location;
- `priority_score`: the importance score used for ranking.

The required ranking rule has two levels:

1. candidates with a higher `priority_score` must appear earlier;
2. if two candidates have the same `priority_score`, the smaller `location_id` must appear earlier.

This rule is implemented once in `CandidateLocation.compareTo()`. The three required sorting algorithms, Bubble Sort, Quick Sort, and Merge Sort, all call this same comparison method. This is important for a fair experiment: if each algorithm implemented its own comparison logic, differences in output might be caused by inconsistent rules instead of algorithm performance.

In Java's `Comparable` convention, `a.compareTo(b) < 0` means `a` should come before `b`. In this project, a higher score therefore produces a smaller comparison result, because higher-priority candidates should be placed at the front of the sorted array.

## 2. Dataset Characteristics

The three candidate files each contain 1000 records, but their initial order is different. This matters because sorting algorithms do not react to input order in the same way.

| Dataset | Main characteristic | Adjacent order breaks | Inversions | Duplicate-score rows |
|---|---:|---:|---:|---:|
| Candidate A | Almost sorted in the required order | 17 | 17 | 0 |
| Candidate B | Random order with distinct scores | 508 | 250696 | 0 |
| Candidate C | Many equal priority scores, partially grouped | 485 | 5995 | 1000 |

An adjacent order break means that two neighbouring records are in the wrong order. For example, if a lower score appears immediately before a higher score, that neighbouring pair is an order break.

An inversion is broader. It counts any pair of records that appears in the wrong relative order, even if the two records are far apart. Bubble Sort is strongly affected by inversions because it can only move records through adjacent swaps.

The table shows three different data shapes:

- Candidate A is nearly sorted. Only a few local mistakes need to be repaired.
- Candidate B is highly disordered. Many records are far from their final position.
- Candidate C has many repeated scores. The score alone is not enough to decide the order, so the tie-breaking rule by `location_id` becomes important.

These characteristics explain why no single basic algorithm is fastest in all three cases.

## 3. Experimental Method

The command-line experiment reads each candidate file, copies the original array before every run, sorts the copy, and records runtime with `System.nanoTime()`. Each algorithm is executed 20 times per dataset, and the output CSV stores every epoch time plus the average time.

Copying the original data before each run is necessary. Otherwise, the first algorithm would sort the array, and the next algorithm would receive already sorted input. That would make the comparison unfair.

The latest measured Task A files used here are:

- `output/taskA_candidate_a_epoch_times_20260518_220459.csv`
- `output/taskA_candidate_b_epoch_times_20260518_220459.csv`
- `output/taskA_candidate_c_epoch_times_20260518_220459.csv`

For small inputs such as 1000 rows, Java micro-timing can be affected by JVM warm-up, garbage collection, and other system activity. Therefore, the numerical results should be interpreted together with algorithmic complexity and dataset characteristics.

## 4. Required Algorithm 1: Bubble Sort

### 4.1 How Bubble Sort Works

Bubble Sort repeatedly scans the array from left to right. During each scan, it compares two neighbouring elements. If the two elements are in the wrong order, it swaps them.

In this project, "wrong order" is decided by `CandidateLocation.compareTo()`. If `data[j].compareTo(data[j + 1]) > 0`, then `data[j]` should come after `data[j + 1]`, so the two elements are swapped.

After one full pass, the largest element according to the comparison direction has moved closer to its final position. Because the project sorts candidates from highest priority to lowest priority, the best candidates gradually move towards the beginning of the array.

The implementation also uses an early-stop optimisation. A boolean flag records whether any swap happened during the current pass. If a full pass finishes with no swap, the array is already sorted, and the algorithm stops immediately.

### 4.2 Why Initial Order Matters

Bubble Sort is very sensitive to how far elements are from their final positions. It only swaps adjacent elements, so an item that belongs near the front but starts near the end must move step by step through many swaps.

This is why inversions matter. A high inversion count means many pairs are in the wrong relative order. Bubble Sort has to repair these mistakes through repeated adjacent swaps.

Candidate A has only 17 inversions. That means the array is already very close to the final ranking. Bubble Sort only needs to repair a small number of local mistakes, and then the early-stop condition ends the algorithm.

Candidate B has 250696 inversions. This is a much harder case for Bubble Sort. Many candidates are far from where they should be, so the algorithm must perform many comparison passes and many swaps.

Candidate C has fewer total inversions than Candidate B, but it still has many adjacent order breaks. The repeated scores also mean that the algorithm frequently has to check the secondary `location_id` rule. Bubble Sort is therefore not as efficient as it is on Candidate A.

### 4.3 Complexity

| Case | Time complexity | Reason |
|---|---:|---|
| Best case | O(n) | The array is already sorted, so one pass detects no swaps. |
| Average case | O(n^2) | Many passes and adjacent comparisons are required. |
| Worst case | O(n^2) | Highly reversed or disordered input requires repeated swaps. |

Space complexity is O(1). Bubble Sort sorts the array in place and only uses a few variables such as indexes, a temporary reference for swapping, and the `swapped` flag.

### 4.4 Result Interpretation

Bubble Sort averaged 151348 ns on Candidate A, which is the best result among the three required algorithms for that dataset. This is expected because Candidate A is nearly sorted.

On Candidate B, Bubble Sort averaged 1965316 ns, making it the slowest required algorithm. This is also expected because random input creates many inversions.

On Candidate C, Bubble Sort averaged 771445 ns. It is faster than on Candidate B but still slower than Merge Sort. The reason is that Candidate C is not fully random, but it still contains many adjacent order breaks and tie comparisons.

## 5. Required Algorithm 2: Quick Sort

### 5.1 How Quick Sort Works

Quick Sort uses divide and conquer. It chooses one element as a pivot, partitions the array around the pivot, and then recursively sorts the two sides.

The project implementation uses Lomuto partitioning:

1. choose the last element of the current range as the pivot;
2. scan the range from left to right;
3. move every element that should come before or equal to the pivot into the left partition;
4. place the pivot between the left and right partitions;
5. recursively sort the two partitions.

This is a real Quick Sort implementation. It is not Selection Sort. Selection Sort would repeatedly search the remaining array for the next minimum or maximum and place it in the next fixed position. The project Quick Sort does not do that. It partitions around a pivot and recursively solves subproblems.

### 5.2 Why Pivot Choice Matters

Quick Sort is fast when the pivot splits the array into two reasonably balanced parts. If each partition is about half the size of the original range, the recursion depth is about log n, and the total work is O(n log n).

However, if the pivot is always the smallest or largest element, one side of the partition becomes empty or nearly empty. The algorithm then only reduces the problem size by one element each time. This creates deep recursion and O(n^2) runtime.

The current project implementation always selects the last element as pivot. This is simple, but it is risky for already sorted or nearly sorted input.

Candidate A is almost already sorted according to the required order. Because the pivot is taken from the end, the pivot is often an extreme value in the current range. The partitions become highly unbalanced, so Quick Sort behaves close to its worst case.

Candidate B is random. The last element is not consistently the smallest or largest element, so the partitions are more balanced in practice. This is why Quick Sort is extremely fast on Candidate B.

Candidate C contains many equal scores. Equal or repeated keys can make partitioning less effective, especially when a simple two-way partition is used. The algorithm still works correctly because `location_id` breaks ties, but the partition structure is less favourable than on Candidate B.

### 5.3 Complexity

| Case | Time complexity | Reason |
|---|---:|---|
| Best case | O(n log n) | Pivots split the array into balanced partitions. |
| Average case | O(n log n) | Random-like data usually avoids extreme partitions. |
| Worst case | O(n^2) | Repeatedly choosing an extreme pivot creates one-sided recursion. |

Space complexity is O(log n) on average because of the recursion stack. In the worst case, if every partition is one-sided, recursion depth can grow to O(n). The implementation does not allocate a temporary array for partitioning.

### 5.4 Result Interpretation

Quick Sort averaged 2543527 ns on Candidate A, which is the worst result among the required algorithms. This does not mean Quick Sort is generally bad. It means this specific fixed-pivot implementation is poorly matched with nearly sorted input.

Quick Sort averaged 52414 ns on Candidate B, making it the fastest required algorithm for the random dataset. The random order gives the fixed pivot a better chance of producing balanced partitions.

Quick Sort averaged 825148 ns on Candidate C, slightly slower than Bubble Sort and much slower than Merge Sort. The repeated-score structure and simple partitioning strategy reduce its advantage.

## 6. Required Algorithm 3: Merge Sort

### 6.1 How Merge Sort Works

Merge Sort also uses divide and conquer, but it divides the array in a more predictable way than Quick Sort.

The algorithm works as follows:

1. split the current array range into two halves;
2. recursively sort the left half;
3. recursively sort the right half;
4. merge the two sorted halves into one sorted range.

The project implementation allocates one temporary `CandidateLocation[]` array and reuses it during merging. During a merge, it compares the first remaining element of the left half with the first remaining element of the right half. The smaller one according to `compareTo()` is copied into the temporary array.

When two elements compare as equal, the implementation chooses the element from the left half first. This makes the merge operation stable for exactly equal comparison values.

### 6.2 Why Merge Sort Is Stable Across Datasets

Merge Sort does not rely on lucky pivot choices or on the input being nearly sorted. It always divides the data into halves. For 1000 records, the number of division levels is roughly log2(1000), and each level processes all records during merging.

This means the work pattern is similar for Candidate A, Candidate B, and Candidate C. A nearly sorted array does not give Merge Sort as much benefit as it gives Bubble Sort, because Merge Sort still divides and merges the whole array. On the other hand, random input does not hurt Merge Sort as much as it hurts Bubble Sort, because merging remains O(n) per level.

For Candidate C, Merge Sort is especially suitable because it handles repeated scores predictably. The sorting rule still uses `location_id` as the second key, and the merge process applies the same comparison rule consistently.

### 6.3 Complexity

| Case | Time complexity | Reason |
|---|---:|---|
| Best case | O(n log n) | Even sorted input is divided and merged. |
| Average case | O(n log n) | There are log n levels and each level processes n elements. |
| Worst case | O(n log n) | The balanced split structure does not depend on input order. |

Space complexity is O(n), because the implementation needs an auxiliary array for merging. The recursion stack also uses O(log n), but the O(n) temporary array is the dominant memory cost.

### 6.4 Result Interpretation

Merge Sort averaged 186795 ns on Candidate A. It is slower than Bubble Sort on this nearly sorted dataset because Bubble Sort can stop early, while Merge Sort still performs full divide-and-merge work.

Merge Sort averaged 973333 ns on Candidate B. It is slower than Quick Sort on the random dataset, but it is much faster than Bubble Sort.

Merge Sort averaged 286593 ns on Candidate C, making it the best required algorithm for the duplicate-heavy dataset. This supports the idea that Merge Sort is a reliable choice when the input contains many equal or similar keys.

## 7. Additional Algorithm: Intro Sort

Intro Sort is included in the project as an extension beyond the three required algorithms. It is useful because it addresses the main weakness of basic Quick Sort.

Intro Sort starts like Quick Sort, but it monitors recursion depth. If the recursion becomes too deep, it switches to Heap Sort for that range. This prevents the O(n^2) worst case caused by repeated bad pivots. It also uses Insertion Sort for small partitions because insertion sort is efficient on small ranges.

In this project:

- the depth limit is based on `2 * floorLog2(n)`;
- the pivot is selected by median-of-three instead of always choosing the last element;
- small partitions are finished with Insertion Sort;
- Heap Sort is used as a fallback when the depth limit is reached.

Intro Sort averaged 277437 ns on Candidate A, 296444 ns on Candidate B, and 371149 ns on Candidate C. These results are not part of the required three-algorithm answer, but they show why production sorting libraries often use introspective sorting: it keeps good average performance while avoiding the worst behaviour of simple Quick Sort.

Its time complexity is O(n log n) in the worst case because of the Heap Sort fallback. Its extra space is normally O(log n) for recursion, while the heap operations are in place.

## 8. Additional Algorithm: Hybrid Adaptive Sort

Hybrid Adaptive Sort is another extension in this project. It is designed around the three coursework data shapes rather than as a general textbook algorithm.

Before sorting, it profiles the input using two measurements:

- `breakRatio`: the percentage of adjacent pairs that are out of order;
- `duplicateScoreRatio`: the percentage of records whose score belongs to a duplicate-score group.

Then it selects a strategy:

| Detected profile | Code condition | Strategy |
|---|---|---|
| Nearly sorted data | `breakRatio <= 0.08` | repair local breaks with insertion-style shifting |
| Many equal scores | `duplicateScoreRatio >= 0.20` | use Merge Sort |
| General unordered data | otherwise | use Intro Sort |

Candidate A has a break ratio of about 0.017, so Hybrid Adaptive Sort treats it as nearly sorted and repairs local breaks. Candidate B has random order and no duplicate scores, so it uses Intro Sort. Candidate C has duplicate scores in every row, so it uses Merge Sort.

This design is easy to explain: first identify the shape of the data, then choose an algorithm that fits that shape. However, profiling is not free. The implementation scans the input and also allocates an integer array of scores for duplicate detection. On small datasets, this overhead can be visible.

Hybrid Adaptive Sort averaged 350141 ns on Candidate A, 291598 ns on Candidate B, and 262200 ns on Candidate C. It is very competitive on Candidate C because it correctly detects the duplicate-heavy structure. On Candidate A, Bubble Sort remains faster because the input is small and already nearly sorted, so the extra profiling cost is not fully recovered.

The time complexity depends on the selected branch. The local repair branch is efficient for nearly sorted data but can approach O(n^2) if many items need shifting. The Merge Sort branch is O(n log n), and the Intro Sort branch is O(n log n) in the worst case. The implementation's space complexity is O(n) because profiling allocates an integer score array, and the Merge Sort branch also requires O(n) auxiliary space.

## 9. Main Experimental Results

The table below compares the three required algorithms.

| Dataset | Bubble Sort avg ns | Quick Sort avg ns | Merge Sort avg ns | Best required algorithm |
|---|---:|---:|---:|---|
| Candidate A | 151348 | 2543527 | 186795 | Bubble Sort |
| Candidate B | 1965316 | 52414 | 973333 | Quick Sort |
| Candidate C | 771445 | 825148 | 286593 | Merge Sort |

The result pattern matches the data characteristics:

- Candidate A rewards algorithms that exploit nearly sorted input, so Bubble Sort wins.
- Candidate B rewards efficient average-case divide-and-conquer behaviour, so Quick Sort wins.
- Candidate C rewards predictable handling of repeated keys, so Merge Sort wins.

This also shows why algorithm selection should not be based only on one dataset. Each algorithm has a different strength and weakness.

## 10. Selected Top 10 Candidates

After sorting, the system selects the first 10 records from each dataset. These are the highest-priority locations according to the required ordering rule.

| Dataset | Selected top 10 locations |
|---|---|
| Candidate A | L0001, L0002, L0003, L0004, L0005, L0006, L0007, L0008, L0009, L0010 |
| Candidate B | L0101, L0102, L0103, L0104, L0105, L0106, L0107, L0108, L0109, L0110 |
| Candidate C | L0201, L0202, L0203, L0204, L0205, L0206, L0207, L0208, L0209, L0210 |

The selected results also show why the secondary key is necessary. In Candidate C, the top candidates all have the same `priority_score` of 5000. Their final order is therefore decided by ascending `location_id`, not by score alone.

These 30 selected locations are the candidates that can be passed to the later routing stage. The sorting stage therefore affects not only runtime performance but also the priority order used by the complete inspection workflow.

## 11. Answers to Required Questions

### 11.1 How does the initial order affect Bubble Sort, Quick Sort, and Merge Sort?

Initial order affects Bubble Sort strongly. When the input is nearly sorted, Bubble Sort only needs a small number of adjacent swaps and can stop early. This is why it performs best on Candidate A. When the input is random, as in Candidate B, many elements are far away from their final positions. Bubble Sort then becomes slow because it can only move elements one position at a time.

Initial order also affects the current Quick Sort implementation strongly, but for a different reason. Quick Sort depends on pivot quality. Since the implementation always chooses the last element as the pivot, nearly sorted input can repeatedly produce poor pivots. Candidate A therefore causes unbalanced partitions and near-worst-case behaviour. Candidate B is random, so the fixed pivot works much better.

Merge Sort is affected least by initial order. It always splits the input into halves and merges them back. The number of levels and the amount of merging work stay similar across the three datasets. This is why Merge Sort is the most consistent required algorithm.

### 11.2 Which of the three sorting algorithms performs best on each dataset?

Candidate A is best handled by Bubble Sort. The data is already close to the final order, with only 17 inversions. Bubble Sort's early-stop optimisation makes it efficient in exactly this situation.

Candidate B is best handled by Quick Sort. The random order prevents the final-element pivot from always being an extreme value, so partitions are more balanced in practice. This allows Quick Sort to use its average-case O(n log n) behaviour.

Candidate C is best handled by Merge Sort. The dataset has many equal priority scores, so predictable tie handling is important. Merge Sort avoids the partitioning weakness that Quick Sort can experience with repeated keys and gives the best measured average time.

### 11.3 Which sorting algorithm behaves most consistently across the three datasets? Why?

Merge Sort behaves most consistently among the three required algorithms. Its performance is based on a fixed divide-and-merge structure rather than on the number of inversions or pivot quality. It does not become extremely fast on nearly sorted data, but it also does not collapse on random or duplicate-heavy data.

Bubble Sort is inconsistent because it is excellent for nearly sorted data but poor for random data. Quick Sort is also inconsistent in this project because the fixed final-element pivot is very fast on Candidate B but very poor on Candidate A.

### 11.4 If only one sorting algorithm could be used in the final system, what criterion should dominate?

The final system should prioritise stable behaviour over the best result on one dataset. In a real inspection system, future input files may not always have the same order pattern as the coursework datasets. A sorting algorithm that is extremely fast in one case but very slow in another case could make the system unreliable.

Among the three required algorithms, Merge Sort is the safest single choice. It gives O(n log n) time in all cases and handles duplicate scores predictably. It uses more memory than Bubble Sort or Quick Sort, but for candidate-location ranking this memory cost is usually acceptable unless the dataset becomes extremely large.

If the extension algorithms are allowed, Intro Sort would be a stronger engineering choice because it combines Quick Sort's average speed with a worst-case protection mechanism.

### 11.5 If the number of candidate locations became significantly larger, which algorithm would be most suitable?

For a much larger dataset, Bubble Sort should not be used. Its O(n^2) behaviour grows too quickly. Doubling the input size can roughly quadruple the amount of work in disordered cases.

Basic Quick Sort is attractive because of its average O(n log n) runtime and low memory usage. However, the current implementation's fixed final-element pivot is risky. For large nearly sorted inputs, it can degrade to O(n^2), which would be unacceptable.

Merge Sort is suitable for large datasets if enough memory is available. Its O(n log n) runtime is predictable, but it requires O(n) additional space.

If the project can use the implemented extension algorithms, Intro Sort is the most suitable large-scale option. It keeps Quick Sort's practical speed, uses limited extra memory, and avoids the worst-case behaviour through Heap Sort fallback.

### 11.6 If both runtime efficiency and memory usage are considered, how would the algorithm choice be affected?

Considering memory makes the decision more balanced. Merge Sort has reliable runtime, but it requires an auxiliary array of size n. If millions of candidate records were sorted on a memory-limited system, this extra space could matter.

Bubble Sort has the best memory usage because it is O(1), but its runtime is too slow for random or large inputs. Low memory usage alone is not enough if the algorithm cannot finish efficiently.

Quick Sort has good average runtime and low average extra memory, but the current pivot strategy makes its worst case risky. A production version should use a better pivot strategy, randomisation, three-way partitioning for duplicates, or Intro Sort.

Therefore:

- choose Merge Sort when predictable runtime and tie handling are the main priorities;
- choose Intro Sort when both runtime and memory efficiency matter;
- use Bubble Sort only for small or nearly sorted inputs;
- avoid the current basic Quick Sort as the only final algorithm unless its pivot strategy is improved.

## 12. Final Recommendation

For the coursework comparison, the best algorithm depends on the dataset: Bubble Sort for Candidate A, Quick Sort for Candidate B, and Merge Sort for Candidate C.

For the final system, a consistent algorithm is more valuable than an algorithm that only wins one data shape. Among the required algorithms, Merge Sort is the recommended final choice because it gives predictable O(n log n) runtime and handles duplicate scores clearly. If the project is allowed to use the additional implementation, Intro Sort is the better engineering choice because it combines good average speed, limited memory use, and worst-case protection.
