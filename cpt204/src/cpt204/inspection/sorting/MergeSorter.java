package cpt204.inspection.sorting;

import cpt204.inspection.model.CandidateLocation;

/**
 * Task A 的归并排序实现。
 */
public class MergeSorter implements Sorter {
    @Override
    public void sort(CandidateLocation[] data) {
        if (data.length <= 1) {
            return;
        }
        CandidateLocation[] temp = new CandidateLocation[data.length];
        mergeSort(data, temp, 0, data.length - 1);
    }

    private void mergeSort(CandidateLocation[] data, CandidateLocation[] temp, int left, int right) {
        if (left >= right) {
            return;
        }
        int mid = left + (right - left) / 2;
        mergeSort(data, temp, left, mid);
        mergeSort(data, temp, mid + 1, right);
        merge(data, temp, left, mid, right);
    }

    private void merge(CandidateLocation[] data, CandidateLocation[] temp, int left, int mid, int right) {
        int i = left;
        int j = mid + 1;
        int k = left;

        while (i <= mid && j <= right) {
            if (data[i].compareTo(data[j]) <= 0) {
                temp[k++] = data[i++];
            } else {
                temp[k++] = data[j++];
            }
        }

        while (i <= mid) {
            temp[k++] = data[i++];
        }
        while (j <= right) {
            temp[k++] = data[j++];
        }
        for (int index = left; index <= right; index++) {
            data[index] = temp[index];
        }
    }

    @Override
    public String getName() {
        return "Merge Sort";
    }
}
