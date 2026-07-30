package cpt204.inspection.sorting;

import cpt204.inspection.model.CandidateLocation;

/**
 * STL-style introspective sort: quicksort first, heapsort fallback, insertion
 * sort for small partitions.
 */
public class IntroSorter implements Sorter {
    private static final int INSERTION_SORT_THRESHOLD = 16;
    private final InsertionSorter insertionSorter = new InsertionSorter();

    @Override
    public void sort(CandidateLocation[] data) {
        if (data.length <= 1) {
            return;
        }

        int depthLimit = 2 * floorLog2(data.length);
        introSort(data, 0, data.length - 1, depthLimit);
        insertionSorter.sortRange(data, 0, data.length);
    }

    private void introSort(CandidateLocation[] data, int low, int high, int depthLimit) {
        while (high - low + 1 > INSERTION_SORT_THRESHOLD) {
            if (depthLimit == 0) {
                heapSortRange(data, low, high);
                return;
            }

            depthLimit--;
            int pivotIndex = partition(data, low, high);
            introSort(data, pivotIndex + 1, high, depthLimit);
            high = pivotIndex - 1;
        }
    }

    private int partition(CandidateLocation[] data, int low, int high) {
        int mid = low + (high - low) / 2;
        int pivotIndex = medianOfThree(data, low, mid, high);
        swap(data, pivotIndex, high);

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

    private int medianOfThree(CandidateLocation[] data, int first, int second, int third) {
        CandidateLocation a = data[first];
        CandidateLocation b = data[second];
        CandidateLocation c = data[third];

        if (a.compareTo(b) < 0) {
            if (b.compareTo(c) < 0) {
                return second;
            }
            return a.compareTo(c) < 0 ? third : first;
        }

        if (a.compareTo(c) < 0) {
            return first;
        }
        return b.compareTo(c) < 0 ? third : second;
    }

    private void heapSortRange(CandidateLocation[] data, int low, int high) {
        int size = high - low + 1;

        for (int i = size / 2 - 1; i >= 0; i--) {
            heapify(data, size, i, low);
        }

        for (int i = size - 1; i > 0; i--) {
            swap(data, low, low + i);
            heapify(data, i, 0, low);
        }
    }

    private void heapify(CandidateLocation[] data, int size, int root, int offset) {
        int largest = root;
        int left = 2 * root + 1;
        int right = 2 * root + 2;

        if (left < size && data[offset + left].compareTo(data[offset + largest]) > 0) {
            largest = left;
        }
        if (right < size && data[offset + right].compareTo(data[offset + largest]) > 0) {
            largest = right;
        }

        if (largest != root) {
            swap(data, offset + root, offset + largest);
            heapify(data, size, largest, offset);
        }
    }

    private int floorLog2(int value) {
        int log = 0;
        while (value > 1) {
            value >>= 1;
            log++;
        }
        return log;
    }

    private void swap(CandidateLocation[] data, int first, int second) {
        CandidateLocation temp = data[first];
        data[first] = data[second];
        data[second] = temp;
    }

    @Override
    public String getName() {
        return "Intro Sort";
    }
}
