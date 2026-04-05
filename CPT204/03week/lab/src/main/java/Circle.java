public class Circle {
    private double x, y, radius;

    public Circle() {
        this.x = this.y = this.radius = 0;
    }

    public double getRedius() {
        return this.radius;
    }

    public double getArea() {
        return radius * radius * Math.PI;
    }

    public double getPerimeter() {
        return 2 * radius * Math.PI;
    }

    public double getDiameter() {
        return 2 * radius;
    }

    public void setRedius(double radius) {
        this.radius = radius;
    }
}
