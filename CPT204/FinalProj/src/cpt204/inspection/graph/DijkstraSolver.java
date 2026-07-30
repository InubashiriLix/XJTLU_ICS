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
 * 用于非负权加权图的 Dijkstra 最短路径求解器。
 */
public class DijkstraSolver implements ShortestPathSolver {
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
        PriorityQueue<NodeDistance> queue = new PriorityQueue<>();

        for (String locationId : graph.getLocationIds()) {
            distances.put(locationId, Double.POSITIVE_INFINITY);
        }
        distances.put(startLocation, 0.0);
        queue.add(new NodeDistance(startLocation, 0.0));

        while (!queue.isEmpty()) {
            NodeDistance current = queue.poll();
            if (current.distance > distances.get(current.locationId)) {
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
                    queue.add(new NodeDistance(edge.getToLocation(), newDistance));
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

    private static class NodeDistance implements Comparable<NodeDistance> {
        private final String locationId;
        private final double distance;

        private NodeDistance(String locationId, double distance) {
            this.locationId = locationId;
            this.distance = distance;
        }

        @Override
        public int compareTo(NodeDistance other) {
            return Double.compare(this.distance, other.distance);
        }
    }
}
