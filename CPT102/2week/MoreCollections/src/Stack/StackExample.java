package Stack;

import java.util.Scanner;

import Stack.ArrayStack;

// read and push onto a stack
// Pop them off the stack
public class StackExample {
    public Scanner scan = new Scanner(System.in);

    public StackExample() {

        System.out.println("input a lis of numbers");
        Stack<Integer> myNums = new ArrayStack<Integer>();
        while (scan.hasNextInt()) {
            int num = scan.nextInt();
            myNums.push(num);
        }

        StringBuilder sb = new StringBuilder("");
        while (!myNums.isEmpty()) {
            sb.append(myNums.pop() + " ");
        }

        System.out.println(sb.toString());

        scan.close();
    }

}
