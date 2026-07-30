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
import java.util.Random;
import java.util.Set;

/**
 * Experimental tabular Q-learning route solver.
 *
 * The learned policy is approximate and does not guarantee the optimal shortest
 * path. It is included to compare reinforcement learning against exact graph
 * algorithms on the same route queries.
 */
public class QLearningRouteSolver implements ShortestPathSolver {
    private static final int TRAINING_EPISODES_PER_DESTINATION = 10000;
    private static final int MAX_EPISODE_STEPS = 250;
    private static final double ALPHA = 0.35;
    private static final double GAMMA = 1.0;
    private static final double EPSILON = 0.25;
    private static final long RANDOM_SEED = 204L;

    private final Map<String, Map<String, Map<String, Double>>> qTablesByDestination = new HashMap<>();
    private final Random random = new Random(RANDOM_SEED);

    public void train(Graph graph, List<String> destinationLocations) {
        List<String> locationIds = new ArrayList<>(graph.getLocationIds());
        Collections.sort(locationIds);

        for (String destination : destinationLocations) {
            if (graph.containsLocation(destination)) {
                trainForDestination(graph, locationIds, destination);
            }
        }
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

        if (!qTablesByDestination.containsKey(endLocation)) {
            train(graph, Collections.singletonList(endLocation));
        }

        Map<String, Map<String, Double>> qTable = qTablesByDestination.get(endLocation);
        List<String> path = new ArrayList<>();
        Set<String> visited = new HashSet<>();
        String current = startLocation;
        double totalCost = 0.0;

        path.add(current);
        visited.add(current);

        for (int step = 0; step < MAX_EPISODE_STEPS; step++) {
            if (current.equals(endLocation)) {
                return new PathResult(path, totalCost, true);
            }

            Edge nextEdge = chooseBestLearnedEdge(graph, qTable, current, visited);
            if (nextEdge == null) {
                return PathResult.unreachable();
            }

            current = nextEdge.getToLocation();
            totalCost += nextEdge.getWeight();
            path.add(current);
            visited.add(current);
        }

        return PathResult.unreachable();
    }

    private void trainForDestination(Graph graph, List<String> locationIds, String destination) {
        Map<String, Map<String, Double>> qTable = createInitialQTable(graph);

        for (int episode = 0; episode < TRAINING_EPISODES_PER_DESTINATION; episode++) {
            String current = locationIds.get(episode % locationIds.size());

            for (int step = 0; step < MAX_EPISODE_STEPS; step++) {
                if (current.equals(destination)) {
                    break;
                }

                Edge action = chooseTrainingEdge(graph, qTable, current);
                if (action == null) {
                    break;
                }

                String next = action.getToLocation();
                double reward = -action.getWeight();
                double oldQ = qTable.get(current).get(next);
                double future = next.equals(destination) ? 0.0 : maxQ(qTable, next);
                double updatedQ = oldQ + ALPHA * (reward + GAMMA * future - oldQ);
                qTable.get(current).put(next, updatedQ);
                current = next;
            }
        }

        qTablesByDestination.put(destination, qTable);
    }

    private Map<String, Map<String, Double>> createInitialQTable(Graph graph) {
        Map<String, Map<String, Double>> qTable = new HashMap<>();

        for (String locationId : graph.getLocationIds()) {
            Map<String, Double> actionValues = new HashMap<>();
            for (Edge edge : graph.getEdgesFrom(locationId)) {
                actionValues.put(edge.getToLocation(), -edge.getWeight());
            }
            qTable.put(locationId, actionValues);
        }

        return qTable;
    }

    private Edge chooseTrainingEdge(Graph graph, Map<String, Map<String, Double>> qTable, String currentLocation) {
        List<Edge> edges = graph.getEdgesFrom(currentLocation);
        if (edges.isEmpty()) {
            return null;
        }

        if (random.nextDouble() < EPSILON) {
            return edges.get(random.nextInt(edges.size()));
        }

        return chooseBestEdgeByQ(edges, qTable.get(currentLocation), Collections.emptySet());
    }

    private Edge chooseBestLearnedEdge(
            Graph graph,
            Map<String, Map<String, Double>> qTable,
            String currentLocation,
            Set<String> visited
    ) {
        List<Edge> edges = graph.getEdgesFrom(currentLocation);
        if (edges.isEmpty()) {
            return null;
        }

        Edge bestUnvisited = chooseBestEdgeByQ(edges, qTable.get(currentLocation), visited);
        if (bestUnvisited != null) {
            return bestUnvisited;
        }

        return chooseBestEdgeByQ(edges, qTable.get(currentLocation), Collections.emptySet());
    }

    private Edge chooseBestEdgeByQ(List<Edge> edges, Map<String, Double> actionValues, Set<String> excludedLocations) {
        Edge bestEdge = null;
        double bestValue = -Double.MAX_VALUE;

        for (Edge edge : edges) {
            if (excludedLocations.contains(edge.getToLocation())) {
                continue;
            }

            double value = actionValues.getOrDefault(edge.getToLocation(), -edge.getWeight());
            if (bestEdge == null || value > bestValue) {
                bestEdge = edge;
                bestValue = value;
            }
        }

        return bestEdge;
    }

    private double maxQ(Map<String, Map<String, Double>> qTable, String locationId) {
        Map<String, Double> actionValues = qTable.get(locationId);
        if (actionValues == null || actionValues.isEmpty()) {
            return 0.0;
        }

        double bestValue = -Double.MAX_VALUE;
        for (double value : actionValues.values()) {
            bestValue = Math.max(bestValue, value);
        }
        return bestValue;
    }
}
