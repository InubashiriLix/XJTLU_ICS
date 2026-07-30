package cpt204.inspection.model;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * 使用邻接表表示的无向加权图。
 */
public class Graph {
    private final Map<String, List<Edge>> adjacencyList = new HashMap<>();

    public void addUndirectedEdge(String fromLocation, String toLocation, double weight) {
        adjacencyList.computeIfAbsent(fromLocation, key -> new ArrayList<>())
                .add(new Edge(fromLocation, toLocation, weight));
        adjacencyList.computeIfAbsent(toLocation, key -> new ArrayList<>())
                .add(new Edge(toLocation, fromLocation, weight));
    }

    public List<Edge> getEdgesFrom(String locationId) {
        return adjacencyList.getOrDefault(locationId, new ArrayList<>());
    }

    public Set<String> getLocationIds() {
        return adjacencyList.keySet();
    }

    public boolean containsLocation(String locationId) {
        return adjacencyList.containsKey(locationId);
    }

    public int size() {
        return adjacencyList.size();
    }
}
