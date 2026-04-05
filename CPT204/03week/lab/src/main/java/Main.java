import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.List;
import java.util.Collections;

class Main {
    public static void main(String[] args) {
        // System.out.println("shit");
        // ==============================================================
        // 3.1 shuffle
        // ArrayList<Number> list = new ArrayList<>(List.of(1, 2, 3, 4, 5, 6, 7));
        // shuffle1(list);
        // printArray(list);
        // ==================================================================
        // 3.2 Comparable Circle
        // var shit = new ComparableCircleTest();
        // ==================================================================
        // 3.3 Cloneable Stack
        // var stack1 = new Stack<Integer>();
        // stack1.push(1);
        // stack1.push(2);
        // stack1.push(3);
        // stack1.push(4);
        // var stack2 = stack1.clone();
        // stack1.pop();
        // System.out.println(stack2);
        // System.out.println(stack1);
        // 3.4 Abastract Employee
        // var shit1 = new FullTimeEmployee("shit", 1);
        // var shit2 = new PartTimeEmployee("shit2", 2);
        // Employee[] list = new Employee[] { shit1.setMonthlySalary(20.0),
        // shit2.setHourlyRate(4).setHoursWorked(50) };
        // for (Employee e : list) {
        // System.out.println(e);
        // }
        // ==================================================================
        // 3.5 shit Discount
        // var shaletamen = new Book("shit book", 100);
        // System.out.println(shaletamen.getDiscountedPrice(20)); // 80
        // ==================================================================
        // 3.6
        // go check the fucking 3.6 markdown
        // ==================================================================
        // 3.7
        // var shitstudent1 = new Student("shit1", 100);
        // var shitstudent2 = new Student("shit2", 80);
        // var shitstudent3 = new Student("shit3", 100);
        // // it should be 1 (greter)
        // System.out.println("shitstudent1 compare to shitstudent2: " +
        // shitstudent1.compareTo(shitstudent2));
        // // it should be smaller
        // System.out.println("shitstudent1 compare to shitstudent3: " +
        // shitstudent1.compareTo(shitstudent3));
        // ==================================================================
        // 3.8
        // Number[] shitNumberArray = new Number[] { 1, 1.0, 2, 2.0, 3.0 };
        // double sum = 0;
        // for (Number e : shitNumberArray) {
        // sum = sum + e.doubleValue();
        // }
        // System.out.println(sum);
    }

    private static <T> void printArray(List<T> list) {
        for (T v : list)
            System.out.print(v + " ");
    }

    // fucking teaccher required
    public static void shuffle1(ArrayList<Number> list) {
        for (int i = 0; i < list.size(); i++)
            Collections.swap(list, i, (int) (Math.random() * list.size()));
    }

    public static <T extends Number> void shuffle2(List<T> list) {
        for (int i = 0; i < list.size(); i++)
            Collections.swap(list, i, (int) (Math.random() * list.size()));
    }

}
