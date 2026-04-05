public class Book extends Product implements Discountable {
    public Book(String name, double price) {
        super(name, price);
    }

    public double getPrice() {
        return price;
    }

    public String getName() {
        return name;
    }

    @Override
    public double getDiscountedPrice(double percent) {
        return price * (1 - percent / 100.0);
    }
}
