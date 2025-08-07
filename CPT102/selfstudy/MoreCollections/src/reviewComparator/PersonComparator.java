package reviewComparator;

import java.util.Comparator;

public class PersonComparator {
    public static Comparator<Person> heightComparator() {
        return new Comparator<Person>() {
            public int compare(Person p1, Person p2) {
                if (p1.height < p2.height) {
                    return -1;
                } else if (p1.height > p2.height) {
                    return 1;
                } else {
                    return 0;
                }

            }
        };
    }

    public static Comparator<Person> weightComparator() {
        return new Comparator<Person>() {
            public int compare(Person p1, Person p2) {
                if (p1.weight < p2.weight) {
                    return -1;
                } else if (p1.weight > p2.weight) {
                    return 1;
                } else {
                    return 0;
                }
            }
        };
    }

    public static Comparator<Person> widthComparator() {
        return new Comparator<Person>() {
            public int compare(Person p1, Person p2) {
                if (p1.width < p2.width) {
                    return -1;
                } else if (p1.width > p2.width) {
                    return 1;
                } else {
                    return 0;
                }
            }
        };

    }

}
