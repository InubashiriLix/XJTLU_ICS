import java.util.ArrayList;
import java.util.Arrays;

public class Main {
    public static <E extends Comparable<E>> E max(ArrayList<E> list) {
        // Empty set
        if (list == null || list.size() == 0) {
            return null;
        }

        // Highest price case
        // E highestPrice = list.get(0);
        // for (int i = 1; i < list.size(); i++) {
        // if (list.get(i).compareTo(highestPrice) > 0) {
        // highestPrice = list.get(i);
        // }
        // }
        // or use collection max method
        var highestPrice = java.util.Collections.max(list);
        // or using stream
        // var highestPrice = list.stream().max(E::compareTo).orElse(null);

        return highestPrice;
    }

    public static void main(String[] args) {
        // printing Test case with an empty list
        ArrayList<Double> prices2 = new ArrayList<>();
        System.out.println(max(prices2)); // -> null

        // Printing Test case with highest prices
        ArrayList<Double> prices1 = new ArrayList<>(
                Arrays.asList(1250000.1, 1750000.3, 2100000.6, 1900000.8, 2300000.2));
        System.out.println(max(prices1));
    }
}
