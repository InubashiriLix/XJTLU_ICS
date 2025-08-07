package reviewComparator;

import java.util.ArrayList;
import java.util.Collections;

public class PersonExample {
    public PersonExample() {
        var list = new ArrayList<Person>();

        list.add(new Person(1, 8, 3));
        list.add(new Person(2, 7, 4));
        list.add(new Person(3, 6, 5));
        list.add(new Person(4, 5, 6));

        // compare default (weight) (from small to height)
        System.out.println("default");
        Collections.sort(list);
        for (var person : list) {
            System.out.println(person.weight);
        }

        // compare height (from small to height)
        System.out.println("height");
        Collections.sort(list, PersonComparator.heightComparator());
        for (var person : list) {
            System.out.println(person.height);
        }

        // compare weight (from small to height)
        System.out.println("weight");
        Collections.sort(list, PersonComparator.weightComparator());
        for (var person : list) {
            System.out.println(person.weight);
        }
        // compare width (from small to height)
        System.out.println("width");
        Collections.sort(list, PersonComparator.widthComparator());
        for (var person : list) {
            System.out.println(person.width);
        }
    }
}
