package cpt204.inspection.sorting;

import cpt204.inspection.model.CandidateLocation;

/**
 * Task A 的冒泡排序实现。
 */
public class BubbleSorter implements Sorter {
    @Override
    public void sort(CandidateLocation[] data) {
        boolean swapped;
        for (int i = 0; i < data.length - 1; i++) {
            swapped = false;
            for (int j = 0; j < data.length - 1 - i; j++) {
                if (data[j].compareTo(data[j + 1]) > 0) {
                    CandidateLocation temp = data[j];
                    data[j] = data[j + 1];
                    data[j + 1] = temp;
                    swapped = true;
                }
            }
            if (!swapped) {
                break;
            }
        }
    }

    @Override
    public String getName() {
        return "Bubble Sort";
    }
}
