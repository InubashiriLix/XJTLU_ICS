public abstract class Employee {
    private String name;
    private Integer id;

    public Employee(String name, Integer id) {
        this.name = name;
        this.id = id;

    }

    public String getName() {
        return this.name;

    }

    public Integer getId() {
        return this.id;
    }

    public abstract double calculatSalary();

    public String toString() {
        return "name: " + this.name + ", id: " + this.id + ", salary: " + this.calculatSalary();
    }
}
