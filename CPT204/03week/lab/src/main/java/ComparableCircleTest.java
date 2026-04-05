public class ComparableCircleTest {
    public ComparableCircleTest() {
        ComparableCircle circle = new ComparableCircle(1.0);
        ComparableCircle circle2 = new ComparableCircle(2.0);
        System.out.println("circle2 > circle: " + circle2.compareTo(circle));
    }
}
