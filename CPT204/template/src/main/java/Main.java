import java.util.Collections;

import ComparableTest.Person.Student;
import IterableTest.Chronos;
import IterableTest.Insertion;

public class Main {
    public static void main(String[] args) {
        Insertion insertion = new Insertion(
                new Chronos[] { new Chronos(1, 30), new Chronos(2, 45), new Chronos(0, 15) });

        Collections.sort(insertion, Chronos.BY_TOTAL_MINUTES);

        for (Chronos shit : insertion) {
            // shit.titik();
            System.out.println(shit);
        }

        Student.main(args);
    }
}
