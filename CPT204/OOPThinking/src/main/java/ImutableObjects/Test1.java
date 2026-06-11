package ImutableObjects;

public class Test1 {
    public static void main() {
        Student student = new Student(1, new BirthDate(1, 1, 2000));
        System.out.println("Student ID: " + student.getId());
        System.out.println("Student Birth Year: " + student.getBirthDate().getYear());
        student.getBirthDate().setYear(1999);
        System.out.println("Student Birth Year: " + student.getBirthDate().getYear());
    }

}
