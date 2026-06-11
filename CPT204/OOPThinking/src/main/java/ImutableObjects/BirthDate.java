package ImutableObjects;

public class BirthDate {
    private final int day;
    private final int month;
    private int year; // NOTE: not final is a problem for immutability, but we intend to change it in
                      // this example

    public BirthDate(int day, int month, int year) {
        this.day = day;
        this.month = month;
        this.year = year;
    }

    public void setYear(int year) {
        this.year = year;
    }

    public int getYear() {
        return year;
    }

}
