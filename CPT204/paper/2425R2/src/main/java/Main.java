import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;

public class Main {
    // Task1: Use the follwing insertionSort() method with the Comparable interface
    // to sort the array of employee IDs in ascending order.

    public static <E extends Comparable<E>> void insertionSort(E[] array) {
        for (int i = 1; i < array.length; i++) {
            E key = array[i];
            int j = i - 1;
            while (j >= 0 && array[j].compareTo(key) > 0) {
                array[j + 1] = array[j];
                j--;
            }
            array[j + 1] = key;
        }
    }

    // Task2: Use the following insertionSort() method with the Comparator interface
    // to srot the array of employee names in a case-insensitive manner
    public static <E> void insertionSort(E[] list, Comparator<? super E> comparator) {
        for (int i = 1; i < list.length; i++) {
            E key = list[i];
            int j = i - 1;
            while (j >= 0 && comparator.compare(list[j], key) > 0) {
                list[j + 1] = list[j];
                j--;
            }
            list[j + 1] = key;
        }
    }

    public static void main(String[] args) {
        Integer[] employeeIds = { 102, 101, 104, 103 };
        String[] employeeNames = { "John", "Alice", "Bob", "Charlie" };

        insertionSort(employeeIds);
        System.out.println("Sorted Employee IDs: ");
        System.out.println(Arrays.toString(employeeIds));

        insertionSort(employeeNames, String.CASE_INSENSITIVE_ORDER);
        System.out.println("Sorted Employee Names: ");
        System.out.println(Arrays.toString(employeeNames));

        ArrayList<String> shit1 = new ArrayList<>(Arrays.asList("shit1", "shit2", "shit3"));
        ArrayList<String> shit2 = new ArrayList<>(Arrays.asList("shit3", "shit2"));
        shit1.remove(shit2);
        shit1.stream().forEach(System.out::println);
    }
}
