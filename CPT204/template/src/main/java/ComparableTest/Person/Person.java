package ComparableTest.Person;

public class Person implements Comparable<Person> {
    private String name;
    private int age;

    Person(String name, int age) {
        this.name = name;
        this.age = age;
    }

    @Override
    public String toString() {
        return "name: " + this.name + " age: " + this.age;
    }

    @Override
    public int compareTo(Person other) {
        return this.age - other.age;
    }
}
