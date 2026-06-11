import java.util.Date;

public class Loan {
    private double annualIntersetRate;
    private int numberOfYears;
    private double loanAmount;
    private Date loanDate;

    public Loan() {
        this(2.5, 1, 1000);

    }

    public Loan(double annualIntersetRate, int numberOfYears, double loanAmount) {
        this.annualIntersetRate = annualIntersetRate;
        this.numberOfYears = numberOfYears;
        this.loanAmount = loanAmount;
        this.loanDate = new Date();
    }

    public double getMonthlyPayment() {
        double monthlyInterestRate = this.annualIntersetRate / 1200;
        return loanAmount * monthlyInterestRate / (1 - (Math.pow(1 / (1 + monthlyInterestRate), numberOfYears * 12)));
    }

    public double getTotalPayment() {
        double totalPayment = getMonthlyPayment() * numberOfYears * 12;
        return totalPayment;
    }
}
