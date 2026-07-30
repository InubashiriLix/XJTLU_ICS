package cpt204.inspection.graph;

import cpt204.inspection.model.Edge;
import cpt204.inspection.model.Graph;
import cpt204.inspection.model.PathResult;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.PriorityQueue;

/**
 * A* shortest path solver for non-negative weighted graphs.
 */
public class AStarSolver implements ShortestPathSolver {
    private final Heuristic heuristic;

    public AStarSolver(Heuristic heuristic) {
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

        Map<String, Double> distances = new HashMap<>();
        Map<String, String> previous = new HashMap<>();
        PriorityQueue<SearchNode> queue = new PriorityQueue<>();

        for (String locationId : graph.getLocationIds()) {
            distances.put(locationId, Double.POSITIVE_INFINITY);
        }

        distances.put(startLocation, 0.0);
        queue.add(new SearchNode(startLocation, 0.0, heuristic.estimate(startLocation, endLocation)));

        while (!queue.isEmpty()) {
            SearchNode current = queue.poll();
            if (current.distanceFromStart > distances.get(current.locationId)) {
                continue;
            }
            if (current.locationId.equals(endLocation)) {
                break;
            }

            for (Edge edge : graph.getEdgesFrom(current.locationId)) {
                double newDistance = distances.get(current.locationId) + edge.getWeight();
                if (newDistance < distances.getOrDefault(edge.getToLocation(), Double.POSITIVE_INFINITY)) {
                    distances.put(edge.getToLocation(), newDistance);
                    previous.put(edge.getToLocation(), current.locationId);

                    double estimatedTotalCost = newDistance
                            + heuristic.estimate(edge.getToLocation(), endLocation);
                    queue.add(new SearchNode(edge.getToLocation(), newDistance, estimatedTotalCost));
                }
            }
        }

        double finalDistance = distances.getOrDefault(endLocation, Double.POSITIVE_INFINITY);
        if (Double.isInfinite(finalDistance)) {
            return PathResult.unreachable();
        }

        return new PathResult(reconstructPath(previous, startLocation, endLocation), finalDistance, true);
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

    private static class SearchNode implements Comparable<SearchNode> {
        private final String locationId;
        private final double distanceFromStart;
        private final double estimatedTotalCost;

        private SearchNode(String locationId, double distanceFromStart, double estimatedTotalCost) {
            this.locationId = locationId;
            this.distanceFromStart = distanceFromStart;
            this.estimatedTotalCost = estimatedTotalCost;
        }

        @Override
        public int compareTo(SearchNode other) {
            int estimateCompare = Double.compare(this.estimatedTotalCost, other.estimatedTotalCost);
            if (estimateCompare != 0) {
                return estimateCompare;
            }
            return Double.compare(this.distanceFromStart, other.distanceFromStart);
        }
    }
}
