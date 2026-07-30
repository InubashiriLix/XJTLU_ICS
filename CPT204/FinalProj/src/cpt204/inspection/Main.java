package cpt204.inspection;

import cpt204.inspection.graph.TaskBExperimentRunner;
import cpt204.inspection.sorting.TaskAExperimentRunner;

/**
 * 应用入口点。
 */
public class Main {
    private static final String[] CANDIDATE_CSV_PATHS = {
            "data/candidates_A.csv",
            "data/candidates_B.csv",
            "data/candidates_C.csv"
    };

    public static void main(String[] args) {
        System.out.println("Urban Infrastructure Inspection System");
        System.out.println("Task A and Task B experiments");
        System.out.println("===========================");

        runTaskAExperiments();
        runTaskBExperiments();
    }

    private static void runTaskAExperiments() {
        TaskAExperimentRunner taskAExperimentRunner = new TaskAExperimentRunner();

        for (String candidateCsvPath : CANDIDATE_CSV_PATHS) {
            try {
                taskAExperimentRunner.runCandidateCsv(candidateCsvPath);
            } catch (Exception e) {
                System.out.println("Error running Task A experiment for " + candidateCsvPath + ": " + e.getMessage());
            }
            System.out.println("===========================");
        }
    }

    private static void runTaskBExperiments() {
        TaskBExperimentRunner taskBExperimentRunner = new TaskBExperimentRunner();

        try {
            taskBExperimentRunner.run();
        } catch (Exception e) {
            System.out.println("Error running Task B experiment: " + e.getMessage());
        }
        System.out.println("===========================");
    }
}
