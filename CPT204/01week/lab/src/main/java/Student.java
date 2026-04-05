public class Student {
    private String firstName = null;
    private String lastName = null;
    private String emailAddress = null;
    private Integer groupNumber = null;

    public Student(String firstName, String lastName, String emailAddress, int groupNumber) {
        if (firstName != null)
            this.firstName = firstName;
        else
            throw new IllegalArgumentException("firstName can not be empty");
        if (lastName != null)
            this.lastName = lastName;
        else
            throw new IllegalArgumentException("lastName can not be empty");
        if (emailAddress != null)
            this.emailAddress = emailAddress;

        else
            throw new IllegalArgumentException("Email Address can not be empty");
        if (groupNumber > 0)
            this.groupNumber = groupNumber;
        else
            throw new IllegalArgumentException("Group Number can not be empty");
    }

    public String toString() {
        return "first_name: " + this.firstName + ", last_name: " + this.lastName + ", emailAddress: "
                + this.emailAddress + ", groupNumber: " + this.groupNumber;
    }

    public boolean less(Student other) {
        return this.groupNumber < other.groupNumber;
    }

}
