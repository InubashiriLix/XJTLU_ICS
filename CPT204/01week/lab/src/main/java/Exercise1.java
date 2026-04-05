import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;

class Exercise1 {
    public void reverseArrayInplace(int[] a) {
        // reverse in place
        int len = a.length;
        for (int i = 0; i < len / 2; i++) {
            int temp = a[i];
            a[i] = a[len - 1 - i];
            a[len - 1 - i] = temp;
        }

        for (int i = 0; i < len; i++)
            System.out.print(a[i] + " ");
    }

    public int[] recordArray() {
        System.out.print("Exercise1\n");
        Scanner scanner = new Scanner(System.in);
        System.out.print("Enter a number: ");
        int number = scanner.nextInt(); // Reads an integer from input
        var ret = new int[number];
        for (int i = 0; i < number; i++)
            ret[i] = scanner.nextInt();
        return ret;
    }

    public boolean isExistValLargerThan4(int[] values) {
        Map<Integer, Integer> histogram = new HashMap<>();
        for (int i = 0; i < values.length; i++) {
            histogram.put(values[i], histogram.getOrDefault(values[i], 0) + 1);
            if (histogram.get(values[i]) >= 4)
                return true;
        }
        return false;
    }

    public boolean isConsecutiveFour(int[] a) {
        int temp = a[0];
        int comboCount = 1;
        for (int i = 1; i < a.length; i++) {
            if (temp == a[i]) {
                comboCount++;
                if (comboCount >= 4)
                    return true;
            } else {
                comboCount = 1;
                temp = a[i];
            }
        }
        return false;
    }

    public int longestRun(int[] v) {
        int longestCnt = 1;
        int currentCnt = 1;
        int LastVal = v[0];
        for (int i = 1; i < v.length; i++) {
            if (LastVal == v[i]) {
                currentCnt++;
                longestCnt = longestCnt < currentCnt ? currentCnt : longestCnt;
            } else {
                currentCnt = 1;
            }
            LastVal = v[i];
        }
        return longestCnt;
    }

}
