import java.util.Arrays;

public class Main {
    public static void main(String[] args) {
        DelimitedARDeque.main(args);

        DelimitedARDeque<Integer> dd1 = new DelimitedARDeque<>(new Integer[] { 1, 2, 3 });
        DelimitedARDeque<Integer> dd2 = new DelimitedARDeque<>(new Integer[] { 1, 4, 3 });
        DelimitedARDeque<Integer> dd3 = new DelimitedARDeque<>(new Integer[] { 2, 4, 3 });
        System.out.println("dd1: " + dd1.toString());
        System.out.println("dd2: " + dd2);
        System.out.println("dd3: " + dd3);
        System.out.println("dd1 equals dd2: " + dd1.equals(dd2));
        System.out.println("dd1 equals dd3: " + dd1.equals(dd3));

    }
}
