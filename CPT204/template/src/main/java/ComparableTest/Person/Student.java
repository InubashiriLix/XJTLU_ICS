package ComparableTest.Person;

import java.util.List;

public class Student extends Person {
    private String studentId;

    Student(String name, int age, String studentId) {
        super(name, age);
        this.studentId = studentId;
    }

    public static <E extends Comparable<? super E>> E max(List<E> list) {
        E result = list.get(0);

        for (E item : list) {
            if (item.compareTo(result) > 0) {
                result = item;
            }
        }

        return result;
    }

    public static void main(String[] args) {
        List<Person> students = List.of(
                new Student("Alice", 20, "S001"),
                new Person("Bob", 22),
                new Student("Charlie", 19, "S002"));

        Person shit = Student.<Person>max(students);
        System.out.println(shit);
    }
}
