public class PartTimeEmployee extends Employee {
    private double hoursWorked;
    private double hourlyRate;

    public PartTimeEmployee(String name, Integer id) {
        super(name, id);
    }

    public PartTimeEmployee setHoursWorked(double v) {
        this.hoursWorked = v;
        return this;

    }

    public PartTimeEmployee setHourlyRate(double v) {
        this.hourlyRate = v;
        return this;
    }

    @Override
    public double calculatSalary() {
        return this.hourlyRate * this.hoursWorked;
    }
}
