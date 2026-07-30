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
import java.util.Set;

/**
 * Dijkstra implementation using linear scans instead of a heap.
 */
public class LinearDijkstraSolver implements ShortestPathSolver {
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
        Set<String> visited = new HashSet<>();

        for (String locationId : graph.getLocationIds()) {
            distances.put(locationId, Double.POSITIVE_INFINITY);
        }
        distances.put(startLocation, 0.0);

        while (visited.size() < graph.size()) {
            String current = findNearestUnvisited(distances, visited);
            if (current == null) {
                break;
            }
            if (current.equals(endLocation)) {
                break;
            }

            visited.add(current);

            for (Edge edge : graph.getEdgesFrom(current)) {
                if (visited.contains(edge.getToLocation())) {
                    continue;
                }

                double newDistance = distances.get(current) + edge.getWeight();
                if (newDistance < distances.getOrDefault(edge.getToLocation(), Double.POSITIVE_INFINITY)) {
                    distances.put(edge.getToLocation(), newDistance);
                    previous.put(edge.getToLocation(), current);
                }
            }
        }

        double finalDistance = distances.getOrDefault(endLocation, Double.POSITIVE_INFINITY);
        if (Double.isInfinite(finalDistance)) {
            return PathResult.unreachable();
        }

        return new PathResult(reconstructPath(previous, startLocation, endLocation), finalDistance, true);
    }

    private String findNearestUnvisited(Map<String, Double> distances, Set<String> visited) {
        String nearestLocation = null;
        double nearestDistance = Double.POSITIVE_INFINITY;

        for (Map.Entry<String, Double> entry : distances.entrySet()) {
            if (!visited.contains(entry.getKey()) && entry.getValue() < nearestDistance) {
                nearestLocation = entry.getKey();
                nearestDistance = entry.getValue();
            }
        }

        return nearestLocation;
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
}
