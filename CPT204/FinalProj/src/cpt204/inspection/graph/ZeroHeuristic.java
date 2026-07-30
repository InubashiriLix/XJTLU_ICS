package cpt204.inspection.graph;

/**
 * A safe fallback heuristic. With this heuristic, A* behaves like Dijkstra.
 */
public class ZeroHeuristic implements Heuristic {
    @Override
    public double estimate(String currentLocation, String destinationLocation) {
        return 0.0;
    }
}
