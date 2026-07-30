package cpt204.inspection.graph;

/**
 * Estimates the remaining path cost between two locations.
 */
public interface Heuristic {
    double estimate(String currentLocation, String destinationLocation);
}
