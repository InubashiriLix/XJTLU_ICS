package cpt204.inspection.graph;

import cpt204.inspection.model.Edge;
import cpt204.inspection.model.Graph;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.PriorityQueue;

/**
 * ALT-style heuristic using distances from selected landmarks.
 *
 * For an undirected graph, |dist(landmark, target) - dist(landmark, current)|
 * is a safe lower bound by the triangle inequality.
 */
public class LandmarkHeuristic implements Heuristic {
    private final List<Map<String, Double>> landmarkDistances = new ArrayList<>();

    public LandmarkHeuristic(Graph graph, List<String> landmarkLocationIds) {
        for (String landmark : landmarkLocationIds) {
            if (graph.containsLocation(landmark)) {
                landmarkDistances.add(computeDistancesFrom(graph, landmark));
            }
        }
    }

    @Override
    public double estimate(String currentLocation, String destinationLocation) {
        double bestLowerBound = 0.0;

        for (Map<String, Double> distances : landmarkDistances) {
            double currentDistance = distances.getOrDefault(currentLocation, Double.POSITIVE_INFINITY);
            double destinationDistance = distances.getOrDefault(destinationLocation, Double.POSITIVE_INFINITY);

            if (!Double.isInfinite(currentDistance) && !Double.isInfinite(destinationDistance)) {
                bestLowerBound = Math.max(bestLowerBound, Math.abs(destinationDistance - currentDistance));
            }
        }

        return bestLowerBound;
    }

    private Map<String, Double> computeDistancesFrom(Graph graph, String startLocation) {
        Map<String, Double> distances = new HashMap<>();
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

            for (Edge edge : graph.getEdgesFrom(current.locationId)) {
                double newDistance = current.distance + edge.getWeight();
                if (newDistance < distances.getOrDefault(edge.getToLocation(), Double.POSITIVE_INFINITY)) {
                    distances.put(edge.getToLocation(), newDistance);
                    queue.add(new NodeDistance(edge.getToLocation(), newDistance));
                }
            }
        }

        return distances;
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
