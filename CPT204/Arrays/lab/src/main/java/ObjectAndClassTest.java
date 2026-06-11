import Objects.Circle;

public class ObjectAndClassTest {
    public static class Foo1 {
        static int cnt = 0;

        public static void increase() {
            cnt++;
        }

        public void shit() {
            System.out.println("shit");
        }

        public static int getCnt() {
            return cnt;
        }
    }

    public static void main() {
        System.out.println("================");
        System.out.println("================");
        System.out.println("================");

        Foo1 f1 = new ObjectAndClassTest.Foo1();
        f1.increase();
        Foo1 f2 = new ObjectAndClassTest.Foo1();
        f2.increase();
        System.out.println("f1.cnt == f2.cnt: " + (f1.getCnt() == f2.getCnt()));

        var c1 = new Circle(1.0);
        var c2 = new Circle(2.0);
        System.out.println("c1's Area < c2's Area: " + (c1.getArea() < c2.getArea()));

    }
}
