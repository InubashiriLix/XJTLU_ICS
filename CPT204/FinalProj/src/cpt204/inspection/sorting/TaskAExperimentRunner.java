package cpt204.inspection.sorting;

import cpt204.inspection.io.CandidateCsvReader;
import cpt204.inspection.model.CandidateLocation;
import cpt204.inspection.util.ResultPrinter;
import cpt204.inspection.util.ResultRecorder;

import java.io.IOException;
import java.nio.file.Path;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.List;

public class TaskAExperimentRunner {
    public void runCandidateCsv(String relativeCsvPath) throws IOException {
        String datasetName = inferDatasetName(relativeCsvPath);
        String outputPath = buildEpochTimesOutputPath(datasetName);

        System.out.println("Running Task A Candidate Sorting Experiment...");
        System.out.println("Dataset: " + datasetName);
        System.out.println("Input CSV: " + relativeCsvPath);
        System.out.println("Output CSV: " + outputPath);

        CandidateCsvReader reader = new CandidateCsvReader();

        List<CandidateLocation> csvLocations = reader.read(relativeCsvPath);
        CandidateLocation[] data = csvLocations.toArray(new CandidateLocation[0]);

        Sorter[] sorters = new Sorter[] {
                new BubbleSorter(),
                new QuickSorter(),
                new MergeSorter(),
                new IntroSorter(),
                new HybridAdaptiveSorter()
        };

        SortingExperiment experiment = new SortingExperiment(sorters);
        List<SortingExperiment.SortingResult> results = experiment.run(data, 20);
        ResultRecorder.writeEpochTimesCsv(outputPath, datasetName, results);

        for (SortingExperiment.SortingResult result : results) {
            System.out.println("Algorithm: " + result.getAlgorithmName());
            System.out.println("Average time ns: " +
                    result.getAverageTimeNs());
            System.out.println("Top 10:");
            ResultPrinter.printTopLocations(result.getTopLocations());
            System.out.println();
        }
    }

    private String inferDatasetName(String csvPath) {
        String fileName = Path.of(csvPath).getFileName().toString();

        if (fileName.endsWith(".csv")) {
            fileName = fileName.substring(0, fileName.length() - 4);
        }

        if (fileName.startsWith("candidates_")) {
            String suffix = fileName.substring("candidates_".length());
            return "Candidate " + suffix.toUpperCase();
        }

        return fileName;
    }

    private String buildEpochTimesOutputPath(String datasetName) {
        String timestamp = LocalDateTime.now()
                .format(DateTimeFormatter.ofPattern("yyyyMMdd_HHmmss"));
        String safeDatasetName = datasetName.toLowerCase().replace(" ", "_");

        return "output/taskA_" + safeDatasetName + "_epoch_times_" + timestamp + ".csv";
    }
}
