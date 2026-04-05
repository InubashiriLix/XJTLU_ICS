import java.lang.reflect.Array;
import java.util.ArrayList;

public class Main {
    private static void increase_arr(int[] src) {
        for (int i = 0; i < Array.getLength(src); i++) {
            src[i] = src[i] + 1;
        }
        // bad example, the for-each iteration will copy each element from src, cause
        // this function can not change orignal elements
        for (int ele : src)
            ele++;
    }

    private static int binary_search_arr(int[] src, int target) {
        int len = Array.getLength(src);
        int start_idx = 0;
        int end_idx = len - 1;

        for (; start_idx <= end_idx;) {
            int mid_idx = (start_idx + end_idx) / 2;
            int val = src[mid_idx];

            if (val > target)
                end_idx = mid_idx;
            else if (val < target)
                start_idx = mid_idx;
            else
                return mid_idx;

            if (start_idx >= end_idx)
                return -1;
        }

        return -1;
    }

    private static <T extends Number> ArrayList<Integer> linear_search(T[] src, T v) {
        ArrayList<Integer> ret = new ArrayList<>();
        for (int i = 0; i < src.length; i++) {
            if (src[i].equals(v)) {
                ret.add(i);
            }
        }
        return ret;
    }

    public static void main(String[] args) {
        System.out.println("Test for creating arrays");

        // how to create java arrays
        // 1. new allocation + default value
        int[] a = new int[3]; // size 3, default value 0
        String[] b = new String[3]; // size3 default value null
        //
        for (var ele : b) {
            if (ele == null)
                System.out.println("is null");
        }
        // 2. Literal value (compiling period syntactic sugar)
        int[] c = new int[] { 1, 2, 3, 4 };
        int[] d = { 1, 2, 3, 4 };

        // multi dimension array
        int[] e = new int[] { 1, 2, 3, 4 };
        int[] f = new int[] { 2, 3, 4, 5 };
        int[][] g = new int[][] { e, f };

        for (var arr : g) {
            System.out.println("\n======");
            for (var ele : arr) {
                System.out.printf("%d", ele);
            }
        }
        System.out.println("\n======");

        // 3. the equal symbol is not clone, but point to array ptr
        int[] h = new int[] { 1, 2, 3, 4, 5 };
        int[] l = new int[] { 2, 3, 4, 5, 6 };
        l = h;
        for (var ele : l)
            System.out.println(ele);
        // print the h instead of l

        // 4. pass array is passing reference
        // buf if your method is impled through for each iteartion, it will not, go
        // check reason in the increase_arr method
        increase_arr(h);
        for (var ele : h)
            System.out.println(ele);

        // 4. linear search
        // for example, we want to find element with specified value:
        var aa = new Integer[] { 1, 1, 2, 3, 4, 5 };
        var target = 1;
        var result = linear_search(aa, target);
        if (result.size() == 0)
            System.out.printf("%d not found", target);
        else {
            System.out.println("found in following index");
            for (var i : result) {
                System.out.println(i);
            }
        }

        // 5. binray_search_arr
        // you should make sure the array is already sorted
        int[] test = new int[] { 1, 2, 3, 4, 5 };
        System.out.printf("the index of number 1 is %d\n", binary_search_arr(test, 1));

        // sort the fucking shit
        int[] test1 = new int[] { 5, 4, 3, 2, 1 };
        // use bubble sort
        for (int i = 0; i < test1.length - 1; i++) {
            for (int j = 0; j < test1.length - 1 - i; j++) {
                if (test1[j] > test1[j + 1]) {
                    // swap
                    int temp = test1[j];
                    test1[j] = test1[j + 1];
                    test1[j + 1] = temp;
                }
            }
        }
        for (var ele : test1) {
            System.out.println(ele);
        }
    }
}
