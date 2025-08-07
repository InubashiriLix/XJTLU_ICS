package Comparable;

import java.lang.Comparable;
import java.util.Comparator;

import Comparable.FaceComparable;

public class FaceComparatorW implements Comparator<FaceComparable> {

    @Override
    public int compare(FaceComparable f1, FaceComparable f2) {
        if (f1.getW() < f2.getW())
            return -1;
        else if (f1.getW() > f2.getW())
            return 1;
        else
            return 0;
    }
}
