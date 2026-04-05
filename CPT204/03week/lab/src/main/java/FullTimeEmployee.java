public class FullTimeEmployee extends Employee {
    private double monthlySalary;

    public FullTimeEmployee(String name, Integer id) {
        super(name, id);
    }

    public FullTimeEmployee setMonthlySalary(double val) {
        this.monthlySalary = val;
        return this;
    }

    @Override
    public double calculatSalary() {
        return this.monthlySalary;

    }
}
