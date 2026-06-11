package ImutableObjects;

public class Student {

    private final int id;
    private final BirthDate birthDate;

    public Student(int id, BirthDate birthDate) {
        this.id = id;
        this.birthDate = birthDate;
    }

    public int getId() {
        return id;
    }

    public BirthDate getBirthDate() {
        return birthDate;
    }
}
