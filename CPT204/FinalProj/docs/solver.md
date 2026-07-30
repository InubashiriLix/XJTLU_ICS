# Task B: Graph Solvers Report

## 1. Task Scope

Task B uses the selected candidate locations from Task A and the infrastructure path file `paths.csv` to answer route queries. The path file is treated as an undirected weighted graph:

- each location is a graph node;
- each CSV row is an undirected edge;
- each edge has a numeric weight representing travel cost.

The graph contains 1000 locations and 2600 undirected edges. In memory, each undirected edge is stored in both directions, so the adjacency list contains 5200 directed edge entries.

The formal route results are produced with `DijkstraSolver`, which is suitable because all path weights in this coursework graph are non-negative. The project also implements additional exact, approximate, and experimental solvers for comparison and GUI demonstration.

## 2. Graph Representation and Shared Route Logic

### Graph

`Graph` stores the network as an adjacency list:

```text
Map<String, List<Edge>> adjacencyList
```

This structure is appropriate because shortest-path algorithms repeatedly need to ask: "from this current node, which edges can I follow next?" An adjacency list answers that efficiently without scanning the whole edge set every time.

### PathResult

Each solver returns a `PathResult` containing:

- `path`: the ordered list of location IDs;
- `totalCost`: the sum of edge weights on the path;
- `reachable`: whether the destination can be reached.

### RouteQueryService

`RouteQueryService` wraps a solver and supports two types of query:

- direct query: start to destination;
- via query: start to waypoint(s), then destination.

For a via query, the service solves each segment in the required order and concatenates the segment paths. When appending a later segment, it skips the first node of that segment to avoid duplicating the waypoint.

Pseudocode:

```text
route = empty list
totalCost = 0
current = start

for next in waypoints followed by destination:
    segment = solver.shortestPath(current, next)
    if segment is unreachable:
        return unreachable

    if route is empty:
        append all nodes from segment.path
    else:
        append segment.path without its first node

    totalCost += segment.totalCost
    current = next

return PathResult(route, totalCost, reachable=true)
```

This means the service optimises each required segment, but it does not reorder waypoints. If the coursework says the route must go through B5 and then C5, the service respects that order.

## 3. Required Route Results

The following formal route outputs come from `output/taskB_route_results_20260518_220459.csv`.

| Case | Start | Destination | Waypoints | Reachable | Total cost |
|---|---|---|---|---|---:|
| Case 1 | L0001 | L0001 | None | true | 0.0 |
| Case 2 | L0001 | L0010 | None | true | 27.0 |
| Case 3 | L0001 | L0101 | L0105 | true | 39.0 |
| Case 4 | L0001 | L0201 | L0105, L0205 | true | 48.0 |

Paths:

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

Case 3 and Case 4 revisit some nodes because the route is constrained by fixed waypoint order. This is not a bug. The service first finds the shortest path to the waypoint, then finds the shortest path from that waypoint to the next target.

## 4. Algorithm Comparison Results

The following averages come from `output/taskB_algorithm_epoch_times_20260518_220500.csv`. Each algorithm was run for 20 epochs on the same four route cases.

| Algorithm | Average query time ns | Precompute time ns | Same costs as Dijkstra |
|---|---:|---:|---|
| Q-Learning Routing (experimental) | 57225 | 264296885 | true |
| Greedy Best-First Learned (experimental) | 116416 | 69363145 | false |
| Greedy Best-First Landmark (approximate) | 402202 | 26665617 | false |
| A* Learned Heuristic (experimental) | 530709 | 69363145 | true |
| Unweighted BFS (approximate) | 994122 | 0 | false |
| A* Landmark | 1234577 | 26665617 | true |
| Heap Dijkstra (PriorityQueue) | 2242236 | 0 | true |
| Bidirectional Dijkstra | 3978780 | 0 | true |
| SPFA | 5495040 | 0 | true |
| Bellman-Ford | 10568929 | 0 | true |
| Linear Dijkstra | 18617508 | 0 | true |

The table should be interpreted carefully. Some algorithms are exact and guarantee the same shortest-path cost as Dijkstra. Others are approximate and may be fast because they solve a simpler problem or follow a heuristic aggressively. Q-learning also has a large training cost before query time, so its very low query time does not mean it is free overall.

## 5. DijkstraSolver

### Description

`DijkstraSolver` is the main formal solver. It is designed for graphs with non-negative edge weights. It keeps the best known distance from the start to every node and repeatedly expands the currently closest unprocessed node.

The implementation uses a `PriorityQueue`, so the next node with the smallest known distance can be selected efficiently. When a shorter route to a neighbour is found, the algorithm updates the neighbour's distance and records the current node in the `previous` map.

### Pseudocode

```text
if start == destination:
    return path [start] with cost 0

set all distances to infinity
distance[start] = 0
push (start, 0) into priority queue

while queue is not empty:
    current = queue.popSmallestDistance()
    if current is an outdated queue entry:
        continue
    if current is destination:
        break

    for each edge current -> neighbour:
        newDistance = distance[current] + edge.weight
        if newDistance < distance[neighbour]:
            distance[neighbour] = newDistance
            previous[neighbour] = current
            push (neighbour, newDistance) into queue

if distance[destination] is infinity:
    return unreachable
else:
    reconstruct path using previous map
```

### Path Construction

The solver starts from the destination and repeatedly follows `previous[current]` until it reaches the start. This creates the path in reverse order, so the list is reversed before returning.

### Complexity

Let V be the number of nodes and E be the number of directed edge entries in the adjacency list.

- Time complexity: O((V + E) log V) in the usual priority-queue analysis.
- Space complexity: O(V + E) working space in this implementation because the queue may contain duplicate stale entries; the distance and previous maps are O(V).

### Suitability

Dijkstra is suitable for the coursework graph because the edge weights are non-negative. It gives exact shortest paths and is efficient enough for the dataset size.

## 6. LinearDijkstraSolver

### Description

`LinearDijkstraSolver` implements the same Dijkstra relaxation idea, but it does not use a heap. Instead, each iteration scans all unvisited nodes to find the node with the smallest known distance.

This makes the algorithm simpler to understand but slower on larger graphs. It is useful as a baseline to show why the heap-based implementation is better.

### Pseudocode

```text
set all distances to infinity
distance[start] = 0
visited = empty set

while not all nodes are visited:
    current = unvisited node with smallest distance
    if current does not exist or current is destination:
        break

    mark current as visited
    for each edge current -> neighbour:
        if neighbour is not visited:
            relax the edge

reconstruct path from previous map
```

### Path Construction

Path construction is the same as heap Dijkstra: follow the `previous` map backward from destination to start, then reverse the result.

### Complexity

- Time complexity: O(V^2 + E). The O(V^2) term comes from repeatedly scanning all nodes to find the nearest unvisited one.
- Space complexity: O(V) for distances, previous nodes, and visited nodes.

## 7. BellmanFordSolver

### Description

Bellman-Ford repeatedly relaxes every edge. After at most V - 1 full passes, all shortest paths without negative cycles have been found. This algorithm is slower than Dijkstra on this coursework graph, but it is important as an alternative because it can handle negative edge weights in general.

The project graph has non-negative weights, so Bellman-Ford is not the best practical choice here. It is included to compare correctness and performance.

### Pseudocode

```text
collect all directed edges
set all distances to infinity
distance[start] = 0

repeat V - 1 times:
    updated = false
    for each edge u -> v:
        if distance[u] + weight(u, v) < distance[v]:
            distance[v] = distance[u] + weight(u, v)
            previous[v] = u
            updated = true
    if updated is false:
        break

if destination is unreachable:
    return unreachable
else:
    reconstruct path
```

### Path Construction

The `previous` map records the predecessor that last improved each node. The path is reconstructed backward from destination to start.

### Complexity

- Time complexity: O(VE) in the worst case.
- Space complexity: O(V + E), because the implementation collects all edges into a list and stores distance and predecessor maps.

## 8. SpfaSolver

### Description

SPFA is a queue-based variant of Bellman-Ford. Instead of scanning every edge on every pass, it only reprocesses nodes whose distance has recently improved. When a node's distance improves, the node is placed in a queue so its outgoing edges can be relaxed later.

SPFA can be faster than Bellman-Ford on many practical graphs, but its worst-case complexity is still O(VE). It is not as predictable as Dijkstra for non-negative weighted graphs.

### Pseudocode

```text
set all distances to infinity
distance[start] = 0
queue = [start]
inQueue = {start}

while queue is not empty:
    current = queue.remove()
    remove current from inQueue

    for each edge current -> neighbour:
        if distance[current] + weight < distance[neighbour]:
            distance[neighbour] = distance[current] + weight
            previous[neighbour] = current
            if neighbour is not in queue:
                add neighbour to queue

reconstruct path
```

### Path Construction

Like Bellman-Ford, SPFA stores `previous[neighbour] = current` whenever an edge relaxation improves the known distance.

### Complexity

- Time complexity: O(VE) in the worst case; often faster in practice.
- Space complexity: O(V) for distance, previous, queue, and in-queue tracking.

## 9. UnweightedBreadthFirstSolver

### Description

`UnweightedBreadthFirstSolver` uses BFS and ignores edge weights when deciding which route to explore. BFS finds the path with the fewest edges, not necessarily the path with the lowest total weight.

After finding a hop-minimal path, the implementation calculates the actual weighted cost of that path. This makes the output comparable with other solvers, but it does not make BFS optimal for weighted graphs.

### Pseudocode

```text
queue = [start]
visited = {start}

while queue is not empty:
    current = queue.remove()
    if current is destination:
        break

    for each edge current -> neighbour:
        if neighbour is unvisited:
            visited.add(neighbour)
            previous[neighbour] = current
            queue.add(neighbour)

if destination was visited:
    reconstruct path and calculate its weighted cost
else:
    return unreachable
```

### Path Construction

BFS records the first predecessor that reaches each node. This produces the fewest-edge path. The weighted cost is calculated afterwards by summing edge weights along the reconstructed path.

### Complexity

- Time complexity: O(V + E).
- Space complexity: O(V).

### Limitation

BFS is exact only when all edges have equal weight or when the graph is truly unweighted. In this coursework graph, it is an approximate comparison algorithm.

## 10. AStarSolver

### Description

A* extends Dijkstra by adding a heuristic estimate of the remaining cost to the destination. Instead of prioritising only the known cost from the start, A* prioritises:

```text
f(n) = g(n) + h(n)
```

where:

- `g(n)` is the known distance from the start to node n;
- `h(n)` is the estimated remaining distance from n to the destination.

If the heuristic never overestimates the true remaining distance, A* remains exact and can expand fewer nodes than Dijkstra.

### Pseudocode

```text
distance[start] = 0
push start with priority heuristic(start, destination)

while queue is not empty:
    current = queue.popSmallestEstimatedTotalCost()
    if current is outdated:
        continue
    if current is destination:
        break

    for each edge current -> neighbour:
        newDistance = distance[current] + edge.weight
        if newDistance < distance[neighbour]:
            distance[neighbour] = newDistance
            previous[neighbour] = current
            priority = newDistance + heuristic(neighbour, destination)
            push neighbour with priority

reconstruct path
```

### Path Construction

A* uses the same predecessor-map reconstruction pattern as Dijkstra.

### Complexity

- Worst-case time complexity: O((V + E) log V), similar to heap Dijkstra.
- Space complexity: O(V + E), mainly for maps and priority queue entries.

With a strong admissible heuristic, practical runtime can improve because fewer nodes are expanded.

## 11. LandmarkHeuristic

### Description

`LandmarkHeuristic` is an ALT-style heuristic. It selects several landmark nodes, precomputes shortest distances from each landmark to all other nodes, and uses the triangle inequality to estimate a lower bound:

```text
h(current, target) = max over landmarks |dist(landmark, target) - dist(landmark, current)|
```

For an undirected graph with non-negative weights, this value is a safe lower bound. Therefore, A* with this heuristic can still return exact shortest paths.

### Pseudocode

```text
precompute:
    for each landmark:
        distancesFromLandmark = Dijkstra(landmark)
        store distancesFromLandmark

estimate(current, target):
    best = 0
    for each landmark distance table:
        if both current and target are reachable:
            bound = abs(dist[landmark][target] - dist[landmark][current])
            best = max(best, bound)
    return best
```

### Complexity

Let L be the number of landmarks.

- Precompute time: O(L(E + V) log V).
- Estimate time: O(L).
- Space complexity: O(LV) for stored landmark distance tables.

### Result Interpretation

In the latest experiment, A* Landmark had a precompute time of 26665617 ns and an average query time of 1234577 ns. It returned the same route costs as Dijkstra.

## 12. LearnedHeuristic

### Description

`LearnedHeuristic` is an experimental Java-only heuristic based on simple linear regression. It uses features such as:

- numeric distance between location IDs;
- whether two nodes are in the same hundred block;
- the landmark lower bound;
- an interaction between ID distance and landmark lower bound.

The model is trained using distances from landmarks. Unlike the landmark heuristic, a learned prediction is not guaranteed to be admissible. If it overestimates the true remaining cost, A* may lose its theoretical optimality guarantee. In this project, benchmark results are checked against Dijkstra using `same_route_costs`.

### Pseudocode

```text
precompute landmark distance tables
build training samples from landmark-to-node distances

for each training epoch:
    for each sample:
        predicted = dot(weights, features)
        error = predicted - target
        update each weight by gradient step

estimate(current, target):
    prediction = dot(weights, features(current, target)) * scale
    if prediction is invalid or negative:
        return 0
    return prediction
```

### Complexity

Let L be landmarks, S be training samples, T be training epochs, and F be feature count.

- Precompute time: O(L(E + V) log V + TSF).
- Estimate time: O(F + L), because the feature calculation includes a landmark lower-bound component.
- Space complexity: O(LV + F).

### Result Interpretation

A* with the learned heuristic averaged 530709 ns after a 69363145 ns precompute step, and it matched Dijkstra costs in the latest experiment. However, it remains experimental because correctness depends on the learned estimates not causing harmful search ordering.

## 13. GreedyBestFirstSolver

### Description

Greedy Best-First Search uses only the heuristic value as priority. It does not add the known cost from the start. Therefore, it tries to move toward nodes that look close to the destination, even if the path already taken is expensive.

This can be very fast, but it does not guarantee the shortest weighted path.

### Pseudocode

```text
queue = start ordered by heuristic(start, destination)
visited = empty set

while queue is not empty:
    current = queue.popSmallestHeuristic()
    if current was visited:
        continue
    mark current visited

    if current is destination:
        reconstruct path and calculate weighted cost

    for each neighbour:
        if neighbour is unvisited:
            previous[neighbour] = current if absent
            push neighbour with priority heuristic(neighbour, destination)

return unreachable
```

### Path Construction

The solver records the first predecessor that discovers each node. After reaching the destination, it reconstructs the path and calculates the actual weighted cost by summing edge weights.

### Complexity

- Time complexity: O((V + E) log V) in a typical priority-queue implementation.
- Space complexity: O(V + E), because the queue may hold many frontier entries.

### Limitation

Greedy search is approximate. In the latest experiment, both Greedy variants had `same_route_costs = false`, meaning they did not always match Dijkstra's optimal route cost.

## 14. BidirectionalDijkstraSolver

### Description

Bidirectional Dijkstra runs two Dijkstra-like searches:

- one forward from the start;
- one backward from the destination.

Because the graph is undirected, the same adjacency list can be used in both directions. The algorithm tracks the best meeting point found by the two searches and stops when the two frontier minimum distances cannot improve the best known route.

### Pseudocode

```text
forward search starts at start
backward search starts at destination
bestDistance = infinity
meeting = none

while both queues are not empty:
    expand one node from forward side
    update bestDistance if the node has backward distance

    expand one node from backward side
    update bestDistance if the node has forward distance

    if forwardQueueMin + backwardQueueMin >= bestDistance:
        break

if meeting was found:
    reconstruct forward part and backward part
else:
    return unreachable
```

### Path Construction

The forward search records predecessors from the start. The backward search records predecessors from the destination side. Reconstruction builds the path from the meeting node back to the start, reverses it, and then appends the path from the meeting node to the destination using the backward predecessor map.

### Complexity

- Worst-case time complexity: O((V + E) log V), similar to Dijkstra.
- Space complexity: O(V + E), because it stores two sets of distances, predecessors, settled sets, and queues.

In practice, it can explore fewer nodes when the start and destination are far apart, but the implementation overhead can make it slower on small graphs.

## 15. QLearningRouteSolver

### Description

`QLearningRouteSolver` is an experimental reinforcement-learning route solver. It learns a Q-value table for each destination. A Q-value estimates how good it is to move from one node to a neighbouring node when trying to reach a particular destination.

The reward is negative edge weight, so shorter edges are preferred. Training uses epsilon-greedy exploration:

- with probability `EPSILON`, choose a random outgoing edge;
- otherwise, choose the edge with the best current Q-value.

After training, query-time routing greedily follows the best learned Q-values while avoiding already visited nodes when possible.

### Pseudocode

```text
train(graph, destinations):
    for each destination:
        initialise Q(current, next) = -edgeWeight
        repeat training episodes:
            choose a start node
            repeat up to max steps:
                choose action by epsilon-greedy policy
                reward = -edge weight
                update Q using:
                    Q = Q + alpha * (reward + gamma * maxFutureQ - Q)
                move to next node

query(start, destination):
    if table for destination is missing:
        train for this destination
    current = start
    path = [start]
    repeat up to max steps:
        choose best learned unvisited neighbour
        move there and add edge weight to total cost
        if destination reached:
            return path
    return unreachable
```

### Path Construction

Unlike Dijkstra-style solvers, Q-learning does not reconstruct from a predecessor map. It constructs the path directly during query execution by following the learned policy from the current node to the next node.

### Complexity

Let D be the number of trained destinations, K be training episodes per destination, S be maximum steps per episode, and deg be average outgoing degree.

- Training time: approximately O(DKS deg).
- Query time: O(S deg), because each step chooses among outgoing edges.
- Space complexity: O(DE), because each destination stores Q-values for graph edges.

### Limitation

Q-learning is experimental here. It can be very fast after training, but it does not provide a mathematical shortest-path guarantee. The latest benchmark matched Dijkstra costs for the four test queries, but this should be interpreted as empirical success on those cases, not as a proof of optimality.

## 16. ZeroHeuristic

### Description

`ZeroHeuristic` always returns 0. If A* uses this heuristic, its priority becomes:

```text
f(n) = g(n) + 0
```

That is exactly Dijkstra's priority rule. Therefore, A* with `ZeroHeuristic` behaves like Dijkstra.

### Complexity

- Estimate time: O(1).
- Space complexity: O(1).
- A* with this heuristic has the same asymptotic complexity as heap Dijkstra.

### Usefulness

It is a safe fallback and a useful correctness check. If a more advanced heuristic is suspected to be incorrect, replacing it with `ZeroHeuristic` should restore Dijkstra-like behaviour.

## 17. Summary of Exact, Approximate, and Experimental Solvers

| Solver | Exact shortest path on this weighted graph? | Main purpose |
|---|---|---|
| DijkstraSolver | Yes | Main formal solver |
| LinearDijkstraSolver | Yes | Baseline without heap |
| BidirectionalDijkstraSolver | Yes | Alternative exact search from both ends |
| BellmanFordSolver | Yes | Alternative that can handle negative edges |
| SpfaSolver | Yes | Queue-based Bellman-Ford variant |
| AStarSolver + LandmarkHeuristic | Yes | Faster exact search with safe heuristic |
| AStarSolver + LearnedHeuristic | Not guaranteed theoretically | Experimental heuristic comparison |
| UnweightedBreadthFirstSolver | No, unless all weights are equal | Unweighted/hop-count baseline |
| GreedyBestFirstSolver | No | Fast heuristic approximation |
| QLearningRouteSolver | No theoretical guarantee | Reinforcement-learning experiment |

## 18. Answers to Report Questions

### 18.1 What graph algorithm did you use, and why is it suitable for the weighted graph?

The formal program uses heap-based Dijkstra as the main shortest-path algorithm. It is suitable because the infrastructure network is a weighted graph with non-negative edge weights. Dijkstra is specifically designed to find exact shortest paths under this condition.

The graph is represented with an adjacency list, which works well with Dijkstra because the algorithm repeatedly expands a node and checks its outgoing edges. The `PriorityQueue` version is more efficient than linear scanning because it quickly selects the unsettled node with the smallest current distance.

### 18.2 How is the chosen algorithm implemented, and what are its complexities?

The implementation stores three main structures:

- `distances`: best known cost from the start to each node;
- `previous`: predecessor used to reconstruct the final path;
- `PriorityQueue<NodeDistance>`: frontier ordered by best known distance.

Each time a shorter path to a neighbour is found, the distance and predecessor are updated, and a new queue entry is inserted. Old queue entries are ignored when polled if their distance is no longer current.

The time complexity is O((V + E) log V), and the working space is O(V + E) in this implementation due to possible duplicate queue entries.

### 18.3 If each shortest-path query is solved optimally, is the whole inspection-planning problem solved optimally?

No. Solving each given shortest-path segment optimally does not automatically solve the entire inspection-planning problem optimally.

For the coursework cases, the waypoint order is fixed. The system finds the shortest path for each segment in that fixed order. This is correct for the specified queries, but a full inspection-planning problem may also need to decide which locations to visit, in what order, with what vehicle/time constraints, and whether revisiting nodes is acceptable. That larger problem is closer to route optimisation or travelling-salesman-style planning, not just a single shortest-path query.

Therefore, local segment optimality is necessary but not sufficient for global planning optimality.

### 18.4 What alternatives could be considered if the graph were unweighted?

If the graph were unweighted, BFS would be the natural choice. BFS finds the path with the fewest edges in O(V + E) time, which is simpler and often faster than Dijkstra.

If all edges had the same weight, BFS would also minimise total cost because each edge contributes equally. In the current weighted graph, however, BFS only minimises hop count and may produce a route with higher total weight.

### 18.5 What alternatives might become more suitable for larger graphs or graphs with coordinates?

For much larger non-negative weighted graphs, heap Dijkstra remains correct but may become expensive if many nodes must be explored. Several alternatives could help:

- A* with a good coordinate-based heuristic, such as Euclidean distance, can reduce search space while preserving optimality if the heuristic is admissible.
- ALT landmark A* can be effective when many queries are run on the same graph, because precomputed landmark distances can accelerate later searches.
- Bidirectional Dijkstra or bidirectional A* can reduce explored nodes for long point-to-point queries.
- Contraction hierarchies or other road-network preprocessing techniques can answer repeated queries very quickly, but they are more complex to implement.
- Q-learning or learned heuristics may be useful for experimentation, but they require training and do not automatically guarantee optimal routes.

The best choice depends on whether the system values one-off simplicity, repeated-query speed, memory usage, or strict optimality guarantees.
