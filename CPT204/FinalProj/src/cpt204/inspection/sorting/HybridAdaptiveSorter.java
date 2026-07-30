package cpt204.inspection.sorting;

import cpt204.inspection.model.CandidateLocation;

import java.util.Arrays;

/**
 * Scenario-aware hybrid sorter for the three coursework data shapes:
 * nearly sorted data, random data, and data with many equal priority scores.
 */
public class HybridAdaptiveSorter implements Sorter {
    private static final double NEARLY_SORTED_BREAK_RATIO = 0.08;
    private static final double DUPLICATE_SCORE_RATIO = 0.20;

    private final InsertionSorter insertionSorter = new InsertionSorter();
    private final MergeSorter mergeSorter = new MergeSorter();
    private final IntroSorter introSorter = new IntroSorter();

    @Override
    public void sort(CandidateLocation[] data) {
        if (data.length <= 1) {
            return;
        }

        DataProfile profile = profile(data);

        if (profile.breakRatio <= NEARLY_SORTED_BREAK_RATIO) {
            repairLocalBreaks(data);
            return;
        }

        if (profile.duplicateScoreRatio >= DUPLICATE_SCORE_RATIO) {
            mergeSorter.sort(data);
            return;
        }

        introSorter.sort(data);
    }

    private void repairLocalBreaks(CandidateLocation[] data) {
        for (int i = 1; i < data.length; i++) {
            if (data[i - 1].compareTo(data[i]) > 0) {
                CandidateLocation current = data[i];
                int j = i - 1;
                while (j >= 0 && data[j].compareTo(current) > 0) {
                    data[j + 1] = data[j];
                    j--;
                }
                data[j + 1] = current;
            }
        }
    }

    private DataProfile profile(CandidateLocation[] data) {
        int orderBreaks = 0;
        for (int i = 0; i < data.length - 1; i++) {
            if (data[i].compareTo(data[i + 1]) > 0) {
                orderBreaks++;
            }
        }

        int[] scores = new int[data.length];
        for (int i = 0; i < data.length; i++) {
            scores[i] = data[i].getPriorityScore();
        }
        Arrays.sort(scores);

        int duplicateScoreCount = 0;
        int currentRun = 1;
        for (int i = 1; i < scores.length; i++) {
            if (scores[i] == scores[i - 1]) {
                currentRun++;
            } else {
                if (currentRun > 1) {
                    duplicateScoreCount += currentRun;
                }
                currentRun = 1;
            }
        }
        if (currentRun > 1) {
            duplicateScoreCount += currentRun;
        }

        return new DataProfile(
                orderBreaks / (double) Math.max(1, data.length - 1),
                duplicateScoreCount / (double) data.length
        );
    }

    @Override
    public String getName() {
        return "Hybrid Adaptive Sort";
    }

    private static class DataProfile {
        private final double breakRatio;
        private final double duplicateScoreRatio;

        private DataProfile(double breakRatio, double duplicateScoreRatio) {
            this.breakRatio = breakRatio;
            this.duplicateScoreRatio = duplicateScoreRatio;
        }
    }
}
