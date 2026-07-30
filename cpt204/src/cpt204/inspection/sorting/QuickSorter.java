package cpt204.inspection.sorting;

import cpt204.inspection.model.CandidateLocation;

/**
 * Task A 的快速排序实现。
 */
public class QuickSorter implements Sorter {
    @Override
    public void sort(CandidateLocation[] data) {
        quickSort(data, 0, data.length - 1);
    }

    private void quickSort(CandidateLocation[] data, int low, int high) {
        if (low < high) {
            int pivotIndex = partition(data, low, high);
            quickSort(data, low, pivotIndex - 1);
            quickSort(data, pivotIndex + 1, high);
        }
    }

    private int partition(CandidateLocation[] data, int low, int high) {
        CandidateLocation pivot = data[high];
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (data[j].compareTo(pivot) <= 0) {
                i++;
                swap(data, i, j);
            }
        }
        swap(data, i + 1, high);
        return i + 1;
    }

    private void swap(CandidateLocation[] data, int i, int j) {
        CandidateLocation temp = data[i];
        data[i] = data[j];
        data[j] = temp;
    }

    @Override
    public String getName() {
        return "Quick Sort";
    }
}
