package cpt204.inspection.model;

/**
 * 表示两个地点之间的带权连接。
 */
public class Edge {
    private final String fromLocation;
    private final String toLocation;
    private final double weight;

    public Edge(String fromLocation, String toLocation, double weight) {
        this.fromLocation = fromLocation;
        this.toLocation = toLocation;
        this.weight = weight;
    }

    public String getFromLocation() {
        return fromLocation;
    }

    public String getToLocation() {
        return toLocation;
    }

    public double getWeight() {
        return weight;
    }

    @Override
    public String toString() {
        return fromLocation + " -> " + toLocation + " (" + weight + ")";
    }
}
