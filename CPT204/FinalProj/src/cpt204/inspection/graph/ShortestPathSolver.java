package cpt204.inspection.graph;

import cpt204.inspection.model.Graph;
import cpt204.inspection.model.PathResult;

/**
 * 最短路径算法的通用抽象接口。
 */
public interface ShortestPathSolver {
    PathResult findShortestPath(Graph graph, String startLocation, String endLocation);
}
