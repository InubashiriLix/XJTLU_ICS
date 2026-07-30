package cpt204.inspection.sorting;

import cpt204.inspection.model.CandidateLocation;
import cpt204.inspection.util.Timer;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * 协调 Task A 的排序实验。
 */
public class SortingExperiment {
    private final Sorter[] sorters;

    public SortingExperiment(Sorter[] sorters) {
        this.sorters = Arrays.copyOf(sorters, sorters.length);
    }

    public List<SortingResult> run(CandidateLocation[] originalData, int repeatTimes) {
        List<SortingResult> results = new ArrayList<>();
        for (Sorter sorter : sorters) {
            long totalTimeNs = 0L;
            CandidateLocation[] finalSortedCopy = null;
            long[] epochTimesNs = new long[repeatTimes];

            for (int i = 0; i < repeatTimes; i++) {
                CandidateLocation[] copy = Arrays.copyOf(originalData, originalData.length);
                Timer timer = new Timer();
                timer.start();
                sorter.sort(copy);

                long elapsedTimeNs = timer.stopNanoseconds();
                epochTimesNs[i] = elapsedTimeNs;
                totalTimeNs += elapsedTimeNs;

                System.out.println(sorter.getName() + " epoch " + (i + 1) + ": " + elapsedTimeNs + " ns");

                finalSortedCopy = copy;
            }

            long averageTimeNs = totalTimeNs / Math.max(1, repeatTimes);
            results.add(new SortingResult(sorter.getName(), averageTimeNs, topN(finalSortedCopy, 10), epochTimesNs));
        }
        return results;
    }

    private CandidateLocation[] topN(CandidateLocation[] sortedData, int n) {
        if (sortedData == null) {
            return new CandidateLocation[0];
        }
        return Arrays.copyOf(sortedData, Math.min(n, sortedData.length));
    }

    public static class SortingResult {
        private final String algorithmName;
        private final long averageTimeNs;
        private final CandidateLocation[] topLocations;
        private final long[] epochTimesNs;

        public SortingResult(String algorithmName, long averageTimeNs, CandidateLocation[] topLocations,
                long[] epochTimesNs) {
            this.algorithmName = algorithmName;
            this.averageTimeNs = averageTimeNs;
            this.topLocations = Arrays.copyOf(topLocations, topLocations.length);
            this.epochTimesNs = Arrays.copyOf(epochTimesNs, epochTimesNs.length);
        }

        public String getAlgorithmName() {
            return algorithmName;
        }

        public long getAverageTimeNs() {
            return averageTimeNs;
        }

        public CandidateLocation[] getTopLocations() {
            return Arrays.copyOf(topLocations, topLocations.length);
        }

        public long[] getEpochTimesNs() {
            return Arrays.copyOf(epochTimesNs, epochTimesNs.length);
        }
    }
}
