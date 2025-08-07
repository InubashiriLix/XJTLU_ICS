package Comparable;

import java.lang.Comparable;
import java.util.Comparator;

import Comparable.FaceComparable;

public class FaceComparatorH implements Comparator<FaceComparable> {

    @Override
    public int compare(FaceComparable f1, FaceComparable f2) {
        if (f1.getH() < f2.getH())
            return -1;
        else if (f1.getH() > f2.getH())
            return 1;
        else
            return 0;
    }
}
