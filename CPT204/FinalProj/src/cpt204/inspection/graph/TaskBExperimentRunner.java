package cpt204.inspection.graph;

import cpt204.inspection.io.CandidateCsvReader;
import cpt204.inspection.io.GraphCsvReader;
import cpt204.inspection.model.CandidateLocation;
import cpt204.inspection.model.Graph;
import cpt204.inspection.model.PathResult;
import cpt204.inspection.sorting.MergeSorter;
import cpt204.inspection.util.ResultRecorder;

import java.io.IOException;
import java.nio.file.Path;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Coordinates Task B shortest path experiments.
 */
public class TaskBExperimentRunner {
    private static final String PATHS_CSV_PATH = "data/paths.csv";
    private static final String CANDIDATE_A_CSV_PATH = "data/candidates_A.csv";
    private static final String CANDIDATE_B_CSV_PATH = "data/candidates_B.csv";
    private static final String CANDIDATE_C_CSV_PATH = "data/candidates_C.csv";
    private static final int TOP_LOCATION_COUNT = 10;
    private static final int GRAPH_ALGORITHM_REPEAT_TIMES = 20;

    private final CandidateCsvReader candidateReader = new CandidateCsvReader();
    private final GraphCsvReader graphReader = new GraphCsvReader();
    private final MergeSorter sorter = new MergeSorter();
    private final RouteQueryService dijkstraRouteQueryService = new RouteQueryService(new DijkstraSolver());

    public void run() throws IOException {
        System.out.println("Task B: Graph algorithm evaluation");

        CandidateLocation[] topA = readTopLocations(CANDIDATE_A_CSV_PATH);
        CandidateLocation[] topB = readTopLocations(CANDIDATE_B_CSV_PATH);
        CandidateLocation[] topC = readTopLocations(CANDIDATE_C_CSV_PATH);

        printTopLocations("Top 10 Dataset A", topA);
        printTopLocations("Top 10 Dataset B", topB);
        printTopLocations("Top 10 Dataset C", topC);

        Graph graph = graphReader.read(PATHS_CSV_PATH);
        List<RouteQuery> queries = buildRequiredQueries(topA, topB, topC);
        TimedRouteQueryRun dijkstraRun = runQueriesWithTiming(graph, queries, dijkstraRouteQueryService);
        List<RouteQueryResult> results = dijkstraRun.getResults();

        printRouteResults(results);

        String outputPath = buildRouteResultsOutputPath();
        ResultRecorder.writeRouteResultsCsv(outputPath, results);
        System.out.println("Task B output CSV: " + outputPath);

        runGraphAlgorithmComparison(graph, queries, dijkstraRun.getResults(), topA, topB, topC);
    }

    private CandidateLocation[] readTopLocations(String csvPath) throws IOException {
        List<CandidateLocation> locations = candidateReader.read(csvPath);
        CandidateLocation[] sortedLocations = locations.toArray(new CandidateLocation[0]);
        sorter.sort(sortedLocations);

        if (sortedLocations.length < TOP_LOCATION_COUNT) {
            throw new IllegalStateException(csvPath + " contains fewer than " + TOP_LOCATION_COUNT + " locations.");
        }

        return Arrays.copyOf(sortedLocations, TOP_LOCATION_COUNT);
    }

    private List<RouteQuery> buildRequiredQueries(
            CandidateLocation[] topA,
            CandidateLocation[] topB,
            CandidateLocation[] topC
    ) {
        String a1 = topA[0].getLocationId();
        String a10 = topA[9].getLocationId();
        String b1 = topB[0].getLocationId();
        String b5 = topB[4].getLocationId();
        String c1 = topC[0].getLocationId();
        String c5 = topC[4].getLocationId();

        List<RouteQuery> queries = new ArrayList<>();
        queries.add(RouteQuery.direct(
                "Case 1",
                "From the 1st selected location in Dataset A to itself",
                a1,
                a1
        ));
        queries.add(RouteQuery.direct(
                "Case 2",
                "From the 1st selected location in Dataset A to the 10th selected location in Dataset A",
                a1,
                a10
        ));
        queries.add(RouteQuery.via(
                "Case 3",
                "From the 1st selected location in Dataset A to the 1st selected location in Dataset B, via the 5th selected location in Dataset B",
                a1,
                b1,
                b5
        ));
        queries.add(RouteQuery.via(
                "Case 4",
                "From the 1st selected location in Dataset A to the 1st selected location in Dataset C, via the 5th selected location in Dataset B and the 5th selected location in Dataset C",
                a1,
                c1,
                b5,
                c5
        ));
        return queries;
    }

    private TimedRouteQueryRun runQueriesWithTiming(
            Graph graph,
            List<RouteQuery> queries,
            RouteQueryService routeQueryService
    ) {
        long startTimeNs = System.nanoTime();
        List<RouteQueryResult> results = runQueries(graph, queries, routeQueryService);
        long elapsedTimeNs = System.nanoTime() - startTimeNs;

        return new TimedRouteQueryRun(results, elapsedTimeNs);
    }

    private List<RouteQueryResult> runQueries(
            Graph graph,
            List<RouteQuery> queries,
            RouteQueryService routeQueryService
    ) {
        List<RouteQueryResult> results = new ArrayList<>();

        for (RouteQuery query : queries) {
            PathResult pathResult;
            String[] waypoints = query.getWaypoints();

            if (waypoints.length == 0) {
                pathResult = routeQueryService.direct(
                        graph,
                        query.getStartLocation(),
                        query.getDestinationLocation()
                );
            } else {
                pathResult = routeQueryService.via(
                        graph,
                        query.getStartLocation(),
                        query.getDestinationLocation(),
                        waypoints
                );
            }

            results.add(new RouteQueryResult(query, pathResult));
        }

        return results;
    }

    private void runGraphAlgorithmComparison(
            Graph graph,
            List<RouteQuery> queries,
            List<RouteQueryResult> baselineResults,
            CandidateLocation[] topA,
            CandidateLocation[] topB,
            CandidateLocation[] topC
    ) throws IOException {
        List<AlgorithmBenchmark> benchmarks = new ArrayList<>();
        benchmarks.add(new AlgorithmBenchmark(
                "Linear Dijkstra",
                new RouteQueryService(new LinearDijkstraSolver()),
                0L
        ));
        benchmarks.add(new AlgorithmBenchmark(
                "Heap Dijkstra (PriorityQueue)",
                new RouteQueryService(new DijkstraSolver()),
                0L
        ));
        benchmarks.add(new AlgorithmBenchmark(
                "Bidirectional Dijkstra",
                new RouteQueryService(new BidirectionalDijkstraSolver()),
                0L
        ));
        benchmarks.add(new AlgorithmBenchmark(
                "SPFA",
                new RouteQueryService(new SpfaSolver()),
                0L
        ));
        benchmarks.add(new AlgorithmBenchmark(
                "Unweighted BFS (approximate)",
                new RouteQueryService(new UnweightedBreadthFirstSolver()),
                0L
        ));

        List<String> landmarks = buildLandmarks(topA, topB, topC);
        long precomputeStartNs = System.nanoTime();
        LandmarkHeuristic landmarkHeuristic = new LandmarkHeuristic(graph, landmarks);
        long precomputeTimeNs = System.nanoTime() - precomputeStartNs;
        benchmarks.add(new AlgorithmBenchmark(
                "A* Landmark",
                new RouteQueryService(new AStarSolver(landmarkHeuristic)),
                precomputeTimeNs
        ));
        benchmarks.add(new AlgorithmBenchmark(
                "Greedy Best-First Landmark (approximate)",
                new RouteQueryService(new GreedyBestFirstSolver(landmarkHeuristic)),
                precomputeTimeNs
        ));

        long learnedPrecomputeStartNs = System.nanoTime();
        LearnedHeuristic learnedHeuristic = new LearnedHeuristic(graph, landmarks);
        long learnedPrecomputeTimeNs = System.nanoTime() - learnedPrecomputeStartNs;
        benchmarks.add(new AlgorithmBenchmark(
                "A* Learned Heuristic (experimental)",
                new RouteQueryService(new AStarSolver(learnedHeuristic)),
                learnedPrecomputeTimeNs
        ));
        benchmarks.add(new AlgorithmBenchmark(
                "Greedy Best-First Learned (experimental)",
                new RouteQueryService(new GreedyBestFirstSolver(learnedHeuristic)),
                learnedPrecomputeTimeNs
        ));

        long qLearningStartNs = System.nanoTime();
        QLearningRouteSolver qLearningRouteSolver = new QLearningRouteSolver();
        qLearningRouteSolver.train(graph, collectRouteDestinations(queries));
        long qLearningPrecomputeTimeNs = System.nanoTime() - qLearningStartNs;
        benchmarks.add(new AlgorithmBenchmark(
                "Q-Learning Routing (experimental)",
                new RouteQueryService(qLearningRouteSolver),
                qLearningPrecomputeTimeNs
        ));
        benchmarks.add(new AlgorithmBenchmark(
                "Bellman-Ford",
                new RouteQueryService(new BellmanFordSolver()),
                0L
        ));

        List<ResultRecorder.AlgorithmTimingResult> timingResults = new ArrayList<>();
        for (AlgorithmBenchmark benchmark : benchmarks) {
            timingResults.add(runBenchmark(graph, queries, baselineResults, benchmark));
        }

        String outputPath = buildGraphAlgorithmTimesOutputPath();
        ResultRecorder.writeAlgorithmEpochTimesCsv(outputPath, "Task B shortest path", timingResults);
        printGraphAlgorithmComparison(landmarks, outputPath, timingResults);
    }

    private List<String> buildLandmarks(
            CandidateLocation[] topA,
            CandidateLocation[] topB,
            CandidateLocation[] topC
    ) {
        List<String> landmarks = new ArrayList<>();
        landmarks.add(topA[0].getLocationId());
        landmarks.add(topA[topA.length - 1].getLocationId());
        landmarks.add(topB[0].getLocationId());
        landmarks.add(topB[topB.length - 1].getLocationId());
        landmarks.add(topC[0].getLocationId());
        landmarks.add(topC[topC.length - 1].getLocationId());
        return landmarks;
    }

    private List<String> collectRouteDestinations(List<RouteQuery> queries) {
        List<String> destinations = new ArrayList<>();

        for (RouteQuery query : queries) {
            for (String waypoint : query.getWaypoints()) {
                if (!destinations.contains(waypoint)) {
                    destinations.add(waypoint);
                }
            }
            if (!destinations.contains(query.getDestinationLocation())) {
                destinations.add(query.getDestinationLocation());
            }
        }

        return destinations;
    }

    private ResultRecorder.AlgorithmTimingResult runBenchmark(
            Graph graph,
            List<RouteQuery> queries,
            List<RouteQueryResult> baselineResults,
            AlgorithmBenchmark benchmark
    ) {
        long[] epochTimesNs = new long[GRAPH_ALGORITHM_REPEAT_TIMES];
        boolean sameRouteCosts = true;

        for (int i = 0; i < GRAPH_ALGORITHM_REPEAT_TIMES; i++) {
            TimedRouteQueryRun run = runQueriesWithTiming(graph, queries, benchmark.getRouteQueryService());
            epochTimesNs[i] = run.getElapsedTimeNs();
            sameRouteCosts = sameRouteCosts && hasSameRouteCosts(baselineResults, run.getResults());
        }

        return new ResultRecorder.AlgorithmTimingResult(
                benchmark.getAlgorithmName(),
                epochTimesNs,
                benchmark.getPrecomputeTimeNs(),
                sameRouteCosts
        );
    }

    private void printGraphAlgorithmComparison(
            List<String> landmarks,
            String outputPath,
            List<ResultRecorder.AlgorithmTimingResult> timingResults
    ) {
        System.out.println("Graph algorithm comparison (" + GRAPH_ALGORITHM_REPEAT_TIMES + " epochs):");
        System.out.println("Landmarks: " + String.join(", ", landmarks));

        for (ResultRecorder.AlgorithmTimingResult result : timingResults) {
            System.out.println(result.getAlgorithmName()
                    + " average query time ns: " + result.getAverageTimeNs()
                    + " | precompute ns: " + result.getPrecomputeTimeNs()
                    + " | same route costs: " + result.hasSameRouteCosts());
        }

        System.out.println("Task B algorithm epoch CSV: " + outputPath);
        System.out.println();
    }

    private boolean hasSameRouteCosts(List<RouteQueryResult> firstResults, List<RouteQueryResult> secondResults) {
        if (firstResults.size() != secondResults.size()) {
            return false;
        }

        for (int i = 0; i < firstResults.size(); i++) {
            PathResult first = firstResults.get(i).getPathResult();
            PathResult second = secondResults.get(i).getPathResult();

            if (first.isReachable() != second.isReachable()) {
                return false;
            }
            if (Math.abs(first.getTotalCost() - second.getTotalCost()) > 0.000001) {
                return false;
            }
        }

        return true;
    }

    private void printTopLocations(String title, CandidateLocation[] locations) {
        System.out.println(title + ":");
        for (int i = 0; i < locations.length; i++) {
            System.out.println((i + 1) + ". " + locations[i]);
        }
        System.out.println();
    }

    private void printRouteResults(List<RouteQueryResult> results) {
        System.out.println("Required shortest path cases:");

        for (RouteQueryResult result : results) {
            RouteQuery query = result.getQuery();
            PathResult pathResult = result.getPathResult();

            System.out.println("---------- " + query.getCaseName() + " ----------");
            System.out.println(query.getDescription());
            System.out.println("Start: " + query.getStartLocation());
            System.out.println("Destination: " + query.getDestinationLocation());
            System.out.println("Waypoint(s): " + query.getWaypointsLabel());
            System.out.println("Reachable: " + pathResult.isReachable());
            System.out.println("Shortest Path: " + result.getPathLabel());
            System.out.println("Total Cost: " + pathResult.getTotalCost());
            System.out.println();
        }
    }

    private String buildRouteResultsOutputPath() {
        String timestamp = LocalDateTime.now()
                .format(DateTimeFormatter.ofPattern("yyyyMMdd_HHmmss"));
        return Path.of("output", "taskB_route_results_" + timestamp + ".csv").toString();
    }

    private String buildGraphAlgorithmTimesOutputPath() {
        String timestamp = LocalDateTime.now()
                .format(DateTimeFormatter.ofPattern("yyyyMMdd_HHmmss"));
        return Path.of("output", "taskB_algorithm_epoch_times_" + timestamp + ".csv").toString();
    }

    private static class TimedRouteQueryRun {
        private final List<RouteQueryResult> results;
        private final long elapsedTimeNs;

        private TimedRouteQueryRun(List<RouteQueryResult> results, long elapsedTimeNs) {
            this.results = results;
            this.elapsedTimeNs = elapsedTimeNs;
        }

        private List<RouteQueryResult> getResults() {
            return results;
        }

        private long getElapsedTimeNs() {
            return elapsedTimeNs;
        }
    }

    private static class AlgorithmBenchmark {
        private final String algorithmName;
        private final RouteQueryService routeQueryService;
        private final long precomputeTimeNs;

        private AlgorithmBenchmark(String algorithmName, RouteQueryService routeQueryService, long precomputeTimeNs) {
            this.algorithmName = algorithmName;
            this.routeQueryService = routeQueryService;
            this.precomputeTimeNs = precomputeTimeNs;
        }

        private String getAlgorithmName() {
            return algorithmName;
        }

        private RouteQueryService getRouteQueryService() {
            return routeQueryService;
        }

        private long getPrecomputeTimeNs() {
            return precomputeTimeNs;
        }
    }
}
