import java.time.Year;

public class Main {
    public static void main(String[] args) {

        Exercise1 exe1 = new Exercise1();
        // 1.1
        // exe1.reverseArrayInplace(exe1.recordArray());
        // ====================================================================
        // 1.2
        // System.out.print(exe1.isConsecutiveFour(exe1.recordArray()));
        // ====================================================================
        // 1.3
        // System.out.print(exe1.longestRun(exe1.recordArray()));
        // ====================================================================
        // 1.4
        // int length = 100000;
        // var temp = new int[length];
        // for (int i = 0; i < length; i++) {
        // temp[i] = length - i;
        // }
        // StopWatch st = new StopWatch();
        // long duration = st.getElapseTime(() -> {
        // for (int i = 0; i < length; i++) {
        // int minIdx = i;
        // for (int j = i + 1; j < length; j++) {
        // if (temp[j] < temp[minIdx])
        // minIdx = j;
        // }
        // int tempVal = temp[i];
        // temp[i] = temp[minIdx];
        // temp[minIdx] = tempVal;
        // }
        // });
        // System.out.println(duration);
        // ====================================================================
        // 1.5
        // var student1 = new Student("Alice", "Smith", "SmithAlice@gmail.com", 1);
        // var student2 = new Student("Bob", "Johnson", "JohnsonBob@gmail.com", 2);
        // System.out.println(student1);
        // System.out.println(student2);
        // System.out.println("Alice < Bob: " + student1.less(student2));
        // ====================================================================
        // 1.6
        // var book1 = new Book("book1", "shit1", Year.of(2023), "2023BigShit");
        // var book2 = new Book("book2", "shit2", Year.of(2024), "2024BigShit");
        // System.out.println(book1);
        // System.out.println(book2);
        // System.out.println(book1.less(book2));
        // ====================================================================
        // 1.7
        // Stock stock = new Stock("ORCL", "Oracle Corporation");
        // stock.previousClosingPrice = 34.5;
        // stock.currentPrice = 34.35;
        // System.out.println("Stock symbol: " + stock.symbol);
        // System.out.println("Stock name: " + stock.name);
        // System.out.println("Previous closing price: " + stock.previousClosingPrice);
        // System.out.println("Current price: " + stock.currentPrice);
        // System.out.printf("Price change percentage: %.2f%%\n",
        // stock.getChangePercent());
    }
}
