package cpt204.inspection.util;

import cpt204.inspection.graph.RouteQuery;
import cpt204.inspection.graph.RouteQueryResult;
import cpt204.inspection.sorting.SortingExperiment;

import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;

/**
 * Writes experiment results to files for reporting and later analysis.
 */
public class ResultRecorder {
    private ResultRecorder() {
    }

    public static void writeEpochTimesCsv(
            String outputPath,
            String datasetName,
            List<SortingExperiment.SortingResult> results
    ) throws IOException {
        Path path = Path.of(outputPath);

        if (path.getParent() != null) {
            Files.createDirectories(path.getParent());
        }

        try (BufferedWriter writer = Files.newBufferedWriter(path)) {
            writer.write("dataset,algorithm,epoch,time_ns,average_time_ns");
            writer.newLine();

            for (SortingExperiment.SortingResult result : results) {
                long[] epochTimes = result.getEpochTimesNs();

                for (int i = 0; i < epochTimes.length; i++) {
                    writer.write(datasetName);
                    writer.write(",");
                    writer.write(result.getAlgorithmName());
                    writer.write(",");
                    writer.write(String.valueOf(i + 1));
                    writer.write(",");
                    writer.write(String.valueOf(epochTimes[i]));
                    writer.write(",");
                    writer.write(String.valueOf(result.getAverageTimeNs()));
                    writer.newLine();
                }
            }
        }
    }

    public static void writeAlgorithmEpochTimesCsv(
            String outputPath,
            String experimentName,
            List<AlgorithmTimingResult> results
    ) throws IOException {
        Path path = Path.of(outputPath);

        if (path.getParent() != null) {
            Files.createDirectories(path.getParent());
        }

        try (BufferedWriter writer = Files.newBufferedWriter(path)) {
            writeCsvRow(
                    writer,
                    "experiment",
                    "algorithm",
                    "epoch",
                    "time_ns",
                    "average_time_ns",
                    "precompute_time_ns",
                    "same_route_costs"
            );

            for (AlgorithmTimingResult result : results) {
                long[] epochTimes = result.getEpochTimesNs();

                for (int i = 0; i < epochTimes.length; i++) {
                    writeCsvRow(
                            writer,
                            experimentName,
                            result.getAlgorithmName(),
                            String.valueOf(i + 1),
                            String.valueOf(epochTimes[i]),
                            String.valueOf(result.getAverageTimeNs()),
                            String.valueOf(result.getPrecomputeTimeNs()),
                            String.valueOf(result.hasSameRouteCosts())
                    );
                }
            }
        }
    }

    public static void writeRouteResultsCsv(
            String outputPath,
            List<RouteQueryResult> results
    ) throws IOException {
        Path path = Path.of(outputPath);

        if (path.getParent() != null) {
            Files.createDirectories(path.getParent());
        }

        try (BufferedWriter writer = Files.newBufferedWriter(path)) {
            writeCsvRow(
                    writer,
                    "case",
                    "description",
                    "start",
                    "destination",
                    "waypoints",
                    "reachable",
                    "total_cost",
                    "path"
            );

            for (RouteQueryResult result : results) {
                RouteQuery query = result.getQuery();
                writeCsvRow(
                        writer,
                        query.getCaseName(),
                        query.getDescription(),
                        query.getStartLocation(),
                        query.getDestinationLocation(),
                        query.getWaypointsLabel(),
                        String.valueOf(result.getPathResult().isReachable()),
                        String.valueOf(result.getPathResult().getTotalCost()),
                        result.getPathLabel()
                );
            }
        }
    }

    private static void writeCsvRow(BufferedWriter writer, String... values) throws IOException {
        for (int i = 0; i < values.length; i++) {
            if (i > 0) {
                writer.write(",");
            }
            writer.write(escapeCsv(values[i]));
        }
        writer.newLine();
    }

    private static String escapeCsv(String value) {
        if (value == null) {
            return "";
        }

        boolean needsQuotes = value.contains(",")
                || value.contains("\"")
                || value.contains("\n")
                || value.contains("\r");

        if (!needsQuotes) {
            return value;
        }

        return "\"" + value.replace("\"", "\"\"") + "\"";
    }

    public static class AlgorithmTimingResult {
        private final String algorithmName;
        private final long[] epochTimesNs;
        private final long averageTimeNs;
        private final long precomputeTimeNs;
        private final boolean sameRouteCosts;

        public AlgorithmTimingResult(
                String algorithmName,
                long[] epochTimesNs,
                long precomputeTimeNs,
                boolean sameRouteCosts
        ) {
            this.algorithmName = algorithmName;
            this.epochTimesNs = epochTimesNs.clone();
            this.averageTimeNs = average(epochTimesNs);
            this.precomputeTimeNs = precomputeTimeNs;
            this.sameRouteCosts = sameRouteCosts;
        }

        public String getAlgorithmName() {
            return algorithmName;
        }

        public long[] getEpochTimesNs() {
            return epochTimesNs.clone();
        }

        public long getAverageTimeNs() {
            return averageTimeNs;
        }

        public long getPrecomputeTimeNs() {
            return precomputeTimeNs;
        }

        public boolean hasSameRouteCosts() {
            return sameRouteCosts;
        }

        private static long average(long[] values) {
            if (values.length == 0) {
                return 0L;
            }

            long total = 0L;
            for (long value : values) {
                total += value;
            }
            return total / values.length;
        }
    }
}
