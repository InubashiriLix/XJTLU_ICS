public class Student implements Comparable<Student> {
    protected String name;
    protected Integer mark;

    public Student(String name, Integer mark) {
        this.name = name;
        this.mark = mark;
    }

    public Integer getMark() {
        return this.mark;
    }

    public String getName() {
        return this.name;
    }

    @Override
    public int compareTo(Student st) {
        var retMark = this.mark.compareTo(st.getMark());
        if (retMark == 0) {
            return this.name.compareTo(st.getName());
        }
        return 1;
    }

}
