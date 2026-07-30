package cpt204.inspection.sorting;

import cpt204.inspection.model.CandidateLocation;

/**
 * Task A 中所有排序算法的通用抽象接口。
 */
public interface Sorter {
    void sort(CandidateLocation[] data);

    String getName();
}
