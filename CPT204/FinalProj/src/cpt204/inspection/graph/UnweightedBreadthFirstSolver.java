package cpt204.inspection.graph;

import cpt204.inspection.model.Edge;
import cpt204.inspection.model.Graph;
import cpt204.inspection.model.PathResult;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Set;

/**
 * BFS path finder that ignores edge weights and minimizes hop count.
 */
public class UnweightedBreadthFirstSolver implements ShortestPathSolver {
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

        Queue<String> queue = new ArrayDeque<>();
        Set<String> visited = new HashSet<>();
        Map<String, String> previous = new HashMap<>();

        queue.add(startLocation);
        visited.add(startLocation);

        while (!queue.isEmpty()) {
            String current = queue.remove();
            if (current.equals(endLocation)) {
                break;
            }

            for (Edge edge : graph.getEdgesFrom(current)) {
                if (!visited.contains(edge.getToLocation())) {
                    visited.add(edge.getToLocation());
                    previous.put(edge.getToLocation(), current);
                    queue.add(edge.getToLocation());
                }
            }
        }

        if (!visited.contains(endLocation)) {
            return PathResult.unreachable();
        }

        List<String> path = reconstructPath(previous, startLocation, endLocation);
        return new PathResult(path, calculateWeightedCost(graph, path), true);
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
}
