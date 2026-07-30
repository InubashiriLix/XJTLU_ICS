package cpt204.inspection.graph;

import cpt204.inspection.model.Edge;
import cpt204.inspection.model.Graph;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.PriorityQueue;

/**
 * Java-only learned heuristic using simple linear regression.
 *
 * This heuristic is experimental. Unlike the landmark lower-bound heuristic, a
 * learned estimate is not guaranteed to be admissible, so benchmark results
 * must be checked against exact Dijkstra costs.
 */
public class LearnedHeuristic implements Heuristic {
    private static final int TRAINING_EPOCHS = 250;
    private static final double LEARNING_RATE = 0.02;

    private final List<Map<String, Double>> landmarkDistances = new ArrayList<>();
    private final double[] weights = new double[5];

    public LearnedHeuristic(Graph graph, List<String> landmarkLocationIds) {
        for (String landmark : landmarkLocationIds) {
            if (graph.containsLocation(landmark)) {
                landmarkDistances.add(computeDistancesFrom(graph, landmark));
            }
        }
        train(graph, landmarkLocationIds);
    }

    @Override
    public double estimate(String currentLocation, String destinationLocation) {
        double prediction = predict(features(currentLocation, destinationLocation));
        if (Double.isNaN(prediction) || prediction < 0.0) {
            return 0.0;
        }
        return prediction;
    }

    private void train(Graph graph, List<String> landmarks) {
        List<TrainingSample> samples = new ArrayList<>();

        for (int i = 0; i < landmarkDistances.size(); i++) {
            String landmark = landmarks.get(i);
            Map<String, Double> distances = landmarkDistances.get(i);

            for (String locationId : graph.getLocationIds()) {
                double distance = distances.getOrDefault(locationId, Double.POSITIVE_INFINITY);
                if (!Double.isInfinite(distance)) {
                    samples.add(new TrainingSample(features(locationId, landmark), distance / 100.0));
                }
            }
        }

        if (samples.isEmpty()) {
            return;
        }

        for (int epoch = 0; epoch < TRAINING_EPOCHS; epoch++) {
            for (TrainingSample sample : samples) {
                double predicted = predictNormalized(sample.features);
                double error = predicted - sample.target;

                for (int i = 0; i < weights.length; i++) {
                    weights[i] -= LEARNING_RATE * error * sample.features[i];
                }
            }
        }
    }

    private double[] features(String currentLocation, String destinationLocation) {
        int currentNumber = parseLocationNumber(currentLocation);
        int destinationNumber = parseLocationNumber(destinationLocation);
        double idDistance = Math.abs(currentNumber - destinationNumber) / 1000.0;
        double sameHundredBlock = currentNumber / 100 == destinationNumber / 100 ? 1.0 : 0.0;
        double landmarkLowerBound = landmarkLowerBound(currentLocation, destinationLocation) / 100.0;

        return new double[] {
                1.0,
                idDistance,
                sameHundredBlock,
                landmarkLowerBound,
                idDistance * landmarkLowerBound
        };
    }

    private double landmarkLowerBound(String currentLocation, String destinationLocation) {
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

    private double predict(double[] features) {
        return predictNormalized(features) * 100.0;
    }

    private double predictNormalized(double[] features) {
        double result = 0.0;
        for (int i = 0; i < weights.length; i++) {
            result += weights[i] * features[i];
        }
        return result;
    }

    private int parseLocationNumber(String locationId) {
        if (locationId == null || locationId.length() < 2) {
            return 0;
        }

        try {
            return Integer.parseInt(locationId.substring(1));
        } catch (NumberFormatException e) {
            return 0;
        }
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

    private static class TrainingSample {
        private final double[] features;
        private final double target;

        private TrainingSample(double[] features, double target) {
            this.features = features;
            this.target = target;
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
