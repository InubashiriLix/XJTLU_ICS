package Exceptions;

public class CatchingExample {
    public static void main(String[] args) {
        int[] numbers = { 1, 2, 3, 4, 5 };
        try {
            System.out.println(numbers[5]);
        } catch (ArrayIndexOutOfBoundsException e) {
            System.out.println("Array index out of bounds: " + e.getMessage());
            // } catch (ZeroDivisionException e) {
            // System.out.println("Zero division error: " + e.getMessage());
        } catch (Exception e) {
            System.out.println("An unexpected error occurred: " + e.getMessage());
        } finally {
            System.out.println("This block always executes.");
        }
    }
}
