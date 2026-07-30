package cpt204.inspection.util;

import cpt204.inspection.model.CandidateLocation;
import cpt204.inspection.model.PathResult;

/**
 * 用于打印报告友好输出的工具方法。
 */
public class ResultPrinter {
    private ResultPrinter() {
        // 工具类；不可实例化。
    }

    public static void printTopLocations(CandidateLocation[] locations) {
        for (int i = 0; i < locations.length; i++) {
            System.out.println((i + 1) + ". " + locations[i]);
        }
    }

    public static void printPathResult(String caseName, PathResult result) {
        System.out.println(caseName + ": " + result);
    }
}
