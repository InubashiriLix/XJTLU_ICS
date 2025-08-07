package reviewComparator;

import java.lang.Comparable;

public class Person implements Comparable<Person> {
    public int height = -1;
    public int width = -1;
    public int weight = -1;

    public Person(int height, int width, int weight) {
        this.height = height;
        this.width = width;
        this.weight = weight;
    }

    public int compareTo(Person other) {
        // compare the weight by default
        if (this.weight < other.weight) {
            return -1;
        } else if (this.weight > other.weight) {
            return 1;
        } else {
            return 0;
        }
    }
}
