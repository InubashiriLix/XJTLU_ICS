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
 * Queue-based Bellman-Ford variant.
 */
public class SpfaSolver implements ShortestPathSolver {
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
        Queue<String> queue = new ArrayDeque<>();
        Set<String> inQueue = new HashSet<>();

        for (String locationId : graph.getLocationIds()) {
            distances.put(locationId, Double.POSITIVE_INFINITY);
        }

        distances.put(startLocation, 0.0);
        queue.add(startLocation);
        inQueue.add(startLocation);

        while (!queue.isEmpty()) {
            String current = queue.remove();
            inQueue.remove(current);

            for (Edge edge : graph.getEdgesFrom(current)) {
                double newDistance = distances.get(current) + edge.getWeight();
                if (newDistance < distances.getOrDefault(edge.getToLocation(), Double.POSITIVE_INFINITY)) {
                    distances.put(edge.getToLocation(), newDistance);
                    previous.put(edge.getToLocation(), current);

                    if (!inQueue.contains(edge.getToLocation())) {
                        queue.add(edge.getToLocation());
                        inQueue.add(edge.getToLocation());
                    }
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
}
