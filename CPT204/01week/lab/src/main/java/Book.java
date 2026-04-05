import java.time.Year;

public class Book {
    private String title;
    private String author;
    private Year year;
    private String isbn;

    public Book(String title, String author, Year year, String isbn) {
        this.title = title;
        this.author = author;
        this.year = year;
        this.isbn = isbn;
    }

    public String toString() {
        return "Title: " + title + ", Author: " + author + ", Year: " + year + ", ISBN: " + isbn;
    }

    public boolean less(Book other) {
        return this.year.isBefore(other.year);
    }

}
