package cpt204.inspection.graph;

import cpt204.inspection.model.Edge;
import cpt204.inspection.model.Graph;
import cpt204.inspection.model.PathResult;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.PriorityQueue;
import java.util.Set;

/**
 * Greedy best-first search using only the heuristic as priority.
 */
public class GreedyBestFirstSolver implements ShortestPathSolver {
    private final Heuristic heuristic;

    public GreedyBestFirstSolver(Heuristic heuristic) {
        this.heuristic = heuristic;
    }

    @Override
    public PathResult findShortestPath(Graph graph, String startLocation, String endLocation) {
        if (startLocation.equals(endLocation)) {
            List<String> path = new ArrayList<>();
            path.add(startLocation);
            return new PathResult(path, 0.0, true);
        }

        if (!graph.containsLocation(startLocation) || !graph.containsLocation(endLocation)) {
            return PathResult.unreachable();
        }

        PriorityQueue<SearchNode> queue = new PriorityQueue<>();
        Set<String> visited = new HashSet<>();
        Map<String, String> previous = new HashMap<>();

        queue.add(new SearchNode(startLocation, heuristic.estimate(startLocation, endLocation)));

        while (!queue.isEmpty()) {
            SearchNode current = queue.poll();
            if (!visited.add(current.locationId)) {
                continue;
            }
            if (current.locationId.equals(endLocation)) {
                List<String> path = reconstructPath(previous, startLocation, endLocation);
                return new PathResult(path, calculateWeightedCost(graph, path), true);
            }

            for (Edge edge : graph.getEdgesFrom(current.locationId)) {
                if (!visited.contains(edge.getToLocation())) {
                    previous.putIfAbsent(edge.getToLocation(), current.locationId);
                    queue.add(new SearchNode(edge.getToLocation(), heuristic.estimate(edge.getToLocation(),
                            endLocation)));
                }
            }
        }

        return PathResult.unreachable();
    }

    private List<String> reconstructPath(Map<String, String> previous, String startLocation, String endLocation) {
        List<String> path = new ArrayList<>();
        String current = endLocation;
        while (current != null) {
            path.add(current);
            if (current.equals(startLocation)) {
                break;
            }
            current = previous.get(current);
        }
        Collections.reverse(path);
        return path;
    }

    private double calculateWeightedCost(Graph graph, List<String> path) {
        double totalCost = 0.0;
        for (int i = 0; i < path.size() - 1; i++) {
            totalCost += findEdgeWeight(graph, path.get(i), path.get(i + 1));
        }
        return totalCost;
    }

    private double findEdgeWeight(Graph graph, String fromLocation, String toLocation) {
        for (Edge edge : graph.getEdgesFrom(fromLocation)) {
            if (edge.getToLocation().equals(toLocation)) {
                return edge.getWeight();
            }
        }
        return Double.POSITIVE_INFINITY;
    }

    private static class SearchNode implements Comparable<SearchNode> {
        private final String locationId;
        private final double heuristicCost;

        private SearchNode(String locationId, double heuristicCost) {
            this.locationId = locationId;
            this.heuristicCost = heuristicCost;
        }

        @Override
        public int compareTo(SearchNode other) {
            return Double.compare(this.heuristicCost, other.heuristicCost);
        }
    }
}
