package cpt204.inspection.model;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * 一次最短路径查询的结果对象。
 */
public class PathResult {
    private final List<String> path;
    private final double totalCost;
    private final boolean reachable;

    public PathResult(List<String> path, double totalCost, boolean reachable) {
        this.path = new ArrayList<>(path);
        this.totalCost = totalCost;
        this.reachable = reachable;
    }

    public static PathResult unreachable() {
        return new PathResult(Collections.emptyList(), Double.POSITIVE_INFINITY, false);
    }

    public List<String> getPath() {
        return new ArrayList<>(path);
    }

    public double getTotalCost() {
        return totalCost;
    }

    public boolean isReachable() {
        return reachable;
    }

    @Override
    public String toString() {
        if (!reachable) {
            return "unreachable";
        }
        return String.join(" -> ", path) + " | cost = " + totalCost;
    }
}
