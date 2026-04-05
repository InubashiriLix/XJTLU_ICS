public class ComparableCircle extends Circle implements Comparable<Circle> {
    public ComparableCircle() {
        super();
    }

    public ComparableCircle(double radius) {
        super();
        setRedius(radius);
    }

    @Override
    public int compareTo(Circle o) {
        return Double.compare(this.getArea(), o.getArea());
    }
}
