package cpt204.inspection.model;

/**
 * 表示来自 candidates_A/B/C.csv 的一个候选地点。
 */
public class CandidateLocation implements Comparable<CandidateLocation> {
    private final String locationId;
    private final int priorityScore;

    public CandidateLocation(String locationId, int priorityScore) {
        this.locationId = locationId;
        this.priorityScore = priorityScore;
    }

    public String getLocationId() {
        return locationId;
    }

    public int getPriorityScore() {
        return priorityScore;
    }

    /**
     * Task A 要求的排序规则：
     * 1. priority_score 更高的在前。
     * 2. 分数相同时，location_id 较小的在前。
     */
    @Override
    public int compareTo(CandidateLocation other) {
        int scoreCompare = Integer.compare(other.priorityScore, this.priorityScore);
        if (scoreCompare != 0) {
            return scoreCompare;
        }
        return this.locationId.compareTo(other.locationId);
    }

    @Override
    public String toString() {
        return locationId + "(" + priorityScore + ")";
    }
}
