package cpt204.inspection.graph;

import cpt204.inspection.model.Edge;
import cpt204.inspection.model.Graph;
import cpt204.inspection.model.PathResult;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Bellman-Ford shortest path solver.
 *
 * It is slower than Dijkstra on this non-negative graph, but it is useful as
 * a comparison algorithm because it can also handle negative edge weights.
 */
public class BellmanFordSolver implements ShortestPathSolver {
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
        List<Edge> edges = collectEdges(graph);

        for (String locationId : graph.getLocationIds()) {
            distances.put(locationId, Double.POSITIVE_INFINITY);
        }
        distances.put(startLocation, 0.0);

        for (int i = 0; i < graph.size() - 1; i++) {
            boolean updated = false;

            for (Edge edge : edges) {
                double fromDistance = distances.getOrDefault(edge.getFromLocation(), Double.POSITIVE_INFINITY);
                if (Double.isInfinite(fromDistance)) {
                    continue;
                }

                double newDistance = fromDistance + edge.getWeight();
                if (newDistance < distances.getOrDefault(edge.getToLocation(), Double.POSITIVE_INFINITY)) {
                    distances.put(edge.getToLocation(), newDistance);
                    previous.put(edge.getToLocation(), edge.getFromLocation());
                    updated = true;
                }
            }

            if (!updated) {
                break;
            }
        }

        double finalDistance = distances.getOrDefault(endLocation, Double.POSITIVE_INFINITY);
        if (Double.isInfinite(finalDistance)) {
            return PathResult.unreachable();
        }

        return new PathResult(reconstructPath(previous, startLocation, endLocation), finalDistance, true);
    }

    private List<Edge> collectEdges(Graph graph) {
        List<Edge> edges = new ArrayList<>();
        for (String locationId : graph.getLocationIds()) {
            edges.addAll(graph.getEdgesFrom(locationId));
        }
        return edges;
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
