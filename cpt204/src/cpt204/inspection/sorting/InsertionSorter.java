package cpt204.inspection.sorting;

import cpt204.inspection.model.CandidateLocation;

/**
 * Insertion sort is efficient for small or nearly sorted arrays.
 */
public class InsertionSorter implements Sorter {
    @Override
    public void sort(CandidateLocation[] data) {
        sortRange(data, 0, data.length);
    }

    public void sortRange(CandidateLocation[] data, int fromInclusive, int toExclusive) {
        for (int i = fromInclusive + 1; i < toExclusive; i++) {
            CandidateLocation current = data[i];
            int j = i - 1;
            while (j >= fromInclusive && data[j].compareTo(current) > 0) {
                data[j + 1] = data[j];
                j--;
            }
            data[j + 1] = current;
        }
    }

    @Override
    public String getName() {
        return "Insertion Sort";
    }
}
