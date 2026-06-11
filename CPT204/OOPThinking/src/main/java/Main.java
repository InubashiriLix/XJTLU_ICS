import ImutableObjects.Test1;

import java.util.stream.IntStream;

import Graphics.GraphicsTest;

public class Main {
    // NOTE: the modern generic method syntax to imple the max
    public static <T extends Comparable<? super T>> T max(T a, T b) {
        return a.compareTo(b) > 0 ? a : b;
    }

    // NOTE: old school max:
    public static Object max1(Comparable a, Comparable b) {
        return (Object) (a.compareTo(b) > 0 ? a : b);
    }

    // NOTE: another old school max with type casting:
    public static Object max2(Object a, Object b) {
        return (Object) (((Comparable) a).compareTo(b) > 0 ? a : b);
    }

    public static void main(String[] args) {
        System.out.printf("Hadaly ");

        Test1.main();
        GraphicsTest.main();

        String s1 = "FUCKYOU";
        String s2 = new String("FUCKYOU");
        String s3 = "FUCKYOU";

        // NOTE: - String Interning is a method of storing only one copy of each
        // distinct
        // compiling time constant /explicit string in the source code stored in a
        // string intern pool
        // - if we use the new operator, then a new object is creteed in the heap
        System.out.println((s1 == s2) + " " + s1.equals(s2));
        System.out.println((s1 == s3) + " " + s1.equals(s3));

        // NOTE: String comparing
        // euqal method has been displayed before.
        System.out.println("Shit".equalsIgnoreCase("shit"));
        System.out.println("abc".compareTo("abd")); // -1 because 'c' is less than 'd'
        System.out.println(("abc".compareToIgnoreCase("ABC")));
        // NOTE: string attributes
        System.out.println("abc".length());
        System.out.println("char index 2 at ABC: " + "ABC".charAt(2));
        System.out.println("concating:");
        var con1 = "cont";
        var con2 = "act";
        System.out.println(con1.concat(con2));

        // NOTE: extracting String
        var shit = "THIS IS A PIECE OF SHIT";
        System.out.println(shit.substring(19));
        System.out.println(shit.substring(19, 22));
        // NOTE: finding a char in String
        System.out.println("char C's index in ABC: " + "ABC".indexOf('C')); // should be 2
        System.out.println("string SHIT's index in ABC: " + shit.indexOf("SHIT")); // should be 19
        System.out.println("string SHIT's index in ABC: " + shit.indexOf("NULL")); // should be -1 (not found)
        // COnverting, Replacing and Splitting Strings
        System.out.println("ABC".toLowerCase());
        System.out.println("abc".toUpperCase());
        System.out.println(" ABC ".trim());
        System.out.println("SHIT".replace('T', 'I'));
        System.out.println(" SHIT IT ".trim().replaceFirst("SHIT", "FUCK"));
        System.out.println(" SHIT IT, SHIT! SHIT! SHIT! ".trim().replaceAll("SHIT", "FUCK"));

        var shitshitshit = "YOU MOTHER FUCKER SHIT".split(" ");
        IntStream.range(0, shitshitshit.length).forEach(i -> System.out.println(shitshitshit[i]));

        Integer aa = 1, ab = 2;
        System.out.println(max(aa, ab));

    }
}
