import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.PriorityQueue;
import java.util.Set;

public class Main {
    public static void main(String[] args) {

        // Task 1: Create PriorityQueues for each section of books
        PriorityQueue<String> fictionQueue = new PriorityQueue<>(Arrays.asList(
                "The Great Gatsby", "To Kill a Mockingbird", "1984",
                "Pride and Prejudice", "Harry Potter", "The Hobbit"));
        PriorityQueue<String> historicalQueue = new PriorityQueue<>(
                Arrays.asList("Sapiens", "Pride and Prejudice", "A Brief History of Time", "Educated"));
        PriorityQueue<String> referenceQueue = new PriorityQueue<>(Arrays.asList(
                "The Great Gatsby", "A Brief History of Time", "Harry Potter",
                "Encyclopedia Britannica", "The Hobbit"));

        // Task2: Call findPopularBooks method and print out the name of the popular
        // books }
        Set<String> popularBooks = findPopularCars(fictionQueue, historicalQueue, referenceQueue);
        System.out.println("Popular Books:");
        System.out.println(popularBooks.size());
        popularBooks.stream().forEach(System.out::println);
    }

    public static Set<String> findPopularCars(
            PriorityQueue<String> fictionQueue,
            PriorityQueue<String> historicalQueue,
            PriorityQueue<String> referenceQueue) {
        // Task 2: Implement a method called "findPopularCars" that are present in all
        // three sections

        // Conver the PriorityQueues to Set for easier Comparison
        Set<String> fictionSet = new HashSet<>(fictionQueue);
        Set<String> historicalSet = new HashSet<>(historicalQueue);
        Set<String> referenceSet = new HashSet<>(referenceQueue);

        return fictionSet.stream().filter(historicalSet::contains).filter(referenceSet::contains)
                .collect(java.util.stream.Collectors.toSet());
    }
}
