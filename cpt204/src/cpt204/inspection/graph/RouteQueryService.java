package cpt204.inspection.graph;

import cpt204.inspection.model.Graph;
import cpt204.inspection.model.PathResult;

import java.util.ArrayList;
import java.util.List;

/**
 * 处理 Task B 的直达和途经点路线查询。
 */
public class RouteQueryService {
    private final ShortestPathSolver solver;

    public RouteQueryService(ShortestPathSolver solver) {
        this.solver = solver;
    }

    public PathResult direct(Graph graph, String start, String destination) {
        return solver.findShortestPath(graph, start, destination);
    }

    public PathResult via(Graph graph, String start, String destination, String... waypoints) {
        List<String> combinedPath = new ArrayList<>();
        double totalCost = 0.0;
        String current = start;

        for (String next : appendDestination(waypoints, destination)) {
            PathResult segment = solver.findShortestPath(graph, current, next);
            if (!segment.isReachable()) {
                return PathResult.unreachable();
            }

            List<String> segmentPath = segment.getPath();
            if (combinedPath.isEmpty()) {
                combinedPath.addAll(segmentPath);
            } else if (segmentPath.size() > 1) {
                combinedPath.addAll(segmentPath.subList(1, segmentPath.size()));
            }
            totalCost += segment.getTotalCost();
            current = next;
        }

        return new PathResult(combinedPath, totalCost, true);
    }

    private String[] appendDestination(String[] waypoints, String destination) {
        String[] routePoints = new String[waypoints.length + 1];
        System.arraycopy(waypoints, 0, routePoints, 0, waypoints.length);
        routePoints[routePoints.length - 1] = destination;
        return routePoints;
    }
}
