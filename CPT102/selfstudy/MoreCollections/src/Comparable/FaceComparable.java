package Comparable;

import java.lang.Comparable;
import java.util.Comparator;

public class FaceComparable implements Comparable<FaceComparable> {
    private double size;
    private double w;
    private double h;

    public double getW() {
        return w;
    }

    public double getH() {
        return h;
    }

    public FaceComparable(double w, double h) {
        this.w = w;
        this.h = h;
    }

    public double size() {
        this.size = w * h;
        return this.size;
    }

    @Override
    public int compareTo(FaceComparable other) {
        if (this.size() < other.size())
            return -1;
        else if (this.size() > other.size())
            return 1;
        else
            return 0;
    }
}
