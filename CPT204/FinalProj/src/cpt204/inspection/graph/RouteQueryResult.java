package cpt204.inspection.graph;

import cpt204.inspection.model.PathResult;

/**
 * Stores the result of one Task B route query.
 */
public class RouteQueryResult {
    private final RouteQuery query;
    private final PathResult pathResult;

    public RouteQueryResult(RouteQuery query, PathResult pathResult) {
        this.query = query;
        this.pathResult = pathResult;
    }

    public RouteQuery getQuery() {
        return query;
    }

    public PathResult getPathResult() {
        return pathResult;
    }

    public String getPathLabel() {
        if (!pathResult.isReachable()) {
            return "No path found";
        }
        return String.join(" -> ", pathResult.getPath());
    }
}
