import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.PriorityQueue;
import java.util.Set;

public class Main {

    public static void main(String[] args) {
        // Task 1: Create PriorityQueues for each class of Passengers
        PriorityQueue<String> firstClassQueue = new PriorityQueue<>(
                Arrays.asList("Alice", "Bob", "Charlie", "Diana", "Ethan", "Fiona"));
        PriorityQueue<String> businessClassQueue = new PriorityQueue<>(
                Arrays.asList("Charlie", "Diana", "Grace", "Ian"));
        PriorityQueue<String> economyClassQueue = new PriorityQueue<>(
                Arrays.asList("Alice", "Grace", "Ethan", "Hannah", "Ian"));

        // Task 3: Call findExclusiveFirstClassPassengers method and print out hte
        // exclusive First class Passengers
        Set<String> exclusiveFirstClassPassengers = findExclusiveFirstClassPassengers(firstClassQueue,
                businessClassQueue, economyClassQueue);
        System.out.println("Exclusive First Class Passengers num: " + exclusiveFirstClassPassengers.size());
        exclusiveFirstClassPassengers.stream().forEach(i -> System.out.printf("%s ", i));
    }

    public static Set<String> findExclusiveFirstClassPassengers(
            PriorityQueue<String> firstClassQueue,
            PriorityQueue<String> businessClassQueue,
            PriorityQueue<String> economyClassQueue) {
        // Task2: Implement this method to find exclusive First Class Passengers;
        // convert the queues into set for easier comparison
        Set<String> exclusiveFirstClass = new HashSet<>(firstClassQueue);
        Set<String> businessClassSet = new HashSet<>(businessClassQueue);
        Set<String> economyClassSet = new HashSet<>(economyClassQueue);
        var retainBusiness = exclusiveFirstClass.removeAll(businessClassSet);
        var retainEconomy = exclusiveFirstClass.removeAll(economyClassSet);

        return exclusiveFirstClass;
    }
}
