public class Stock {
    String symbol;
    String name;
    double previousClosingPrice;
    double currentPrice;

    // constructor
    public Stock(String symbol, String name) {
        this.symbol = symbol;
        this.name = name;
    }

    // method to calculate percentage change
    public double getChangePercent() {
        return (currentPrice - previousClosingPrice) / previousClosingPrice * 100;
    }
}
