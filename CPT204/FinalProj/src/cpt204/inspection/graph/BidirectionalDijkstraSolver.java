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
 * Bidirectional Dijkstra for undirected non-negative weighted graphs.
 */
public class BidirectionalDijkstraSolver implements ShortestPathSolver {
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

        SearchState forward = new SearchState(graph, startLocation);
        SearchState backward = new SearchState(graph, endLocation);
        String meetingLocation = null;
        double bestDistance = Double.POSITIVE_INFINITY;

        while (!forward.queue.isEmpty() && !backward.queue.isEmpty()) {
            bestDistance = expand(graph, forward, backward, bestDistance);
            meetingLocation = chooseMeetingLocation(forward.distances, backward.distances, bestDistance, meetingLocation);

            bestDistance = expand(graph, backward, forward, bestDistance);
            meetingLocation = chooseMeetingLocation(forward.distances, backward.distances, bestDistance, meetingLocation);

            double forwardMin = forward.queue.peek() == null ? Double.POSITIVE_INFINITY : forward.queue.peek().distance;
            double backwardMin = backward.queue.peek() == null ? Double.POSITIVE_INFINITY : backward.queue.peek().distance;
            if (forwardMin + backwardMin >= bestDistance) {
                break;
            }
        }

        if (meetingLocation == null || Double.isInfinite(bestDistance)) {
            return PathResult.unreachable();
        }

        return new PathResult(reconstructPath(forward.previous, backward.previous, startLocation, endLocation,
                meetingLocation), bestDistance, true);
    }

    private double expand(Graph graph, SearchState currentSide, SearchState otherSide, double bestDistance) {
        NodeDistance current = currentSide.queue.poll();
        if (current == null || current.distance > currentSide.distances.get(current.locationId)) {
            return bestDistance;
        }

        currentSide.settled.add(current.locationId);

        if (otherSide.distances.containsKey(current.locationId)) {
            bestDistance = Math.min(bestDistance,
                    currentSide.distances.get(current.locationId) + otherSide.distances.get(current.locationId));
        }

        for (Edge edge : graph.getEdgesFrom(current.locationId)) {
            if (currentSide.settled.contains(edge.getToLocation())) {
                continue;
            }

            double newDistance = currentSide.distances.get(current.locationId) + edge.getWeight();
            if (newDistance < currentSide.distances.getOrDefault(edge.getToLocation(), Double.POSITIVE_INFINITY)) {
                currentSide.distances.put(edge.getToLocation(), newDistance);
                currentSide.previous.put(edge.getToLocation(), current.locationId);
                currentSide.queue.add(new NodeDistance(edge.getToLocation(), newDistance));
            }
        }

        return bestDistance;
    }

    private String chooseMeetingLocation(
            Map<String, Double> forwardDistances,
            Map<String, Double> backwardDistances,
            double bestDistance,
            String currentMeetingLocation
    ) {
        String meetingLocation = currentMeetingLocation;
        for (Map.Entry<String, Double> entry : forwardDistances.entrySet()) {
            double backwardDistance = backwardDistances.getOrDefault(entry.getKey(), Double.POSITIVE_INFINITY);
            if (Math.abs(entry.getValue() + backwardDistance - bestDistance) < 0.000001) {
                meetingLocation = entry.getKey();
                break;
            }
        }
        return meetingLocation;
    }

    private List<String> reconstructPath(
            Map<String, String> forwardPrevious,
            Map<String, String> backwardPrevious,
            String startLocation,
            String endLocation,
            String meetingLocation
    ) {
        List<String> path = new ArrayList<>();
        String current = meetingLocation;

        while (current != null) {
            path.add(current);
            if (current.equals(startLocation)) {
                break;
            }
            current = forwardPrevious.get(current);
        }
        Collections.reverse(path);

        current = backwardPrevious.get(meetingLocation);
        while (current != null) {
            path.add(current);
            if (current.equals(endLocation)) {
                break;
            }
            current = backwardPrevious.get(current);
        }

        return path;
    }

    private static class SearchState {
        private final Map<String, Double> distances = new HashMap<>();
        private final Map<String, String> previous = new HashMap<>();
        private final Set<String> settled = new HashSet<>();
        private final PriorityQueue<NodeDistance> queue = new PriorityQueue<>();

        private SearchState(Graph graph, String startLocation) {
            for (String locationId : graph.getLocationIds()) {
                distances.put(locationId, Double.POSITIVE_INFINITY);
            }
            distances.put(startLocation, 0.0);
            queue.add(new NodeDistance(startLocation, 0.0));
        }
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
