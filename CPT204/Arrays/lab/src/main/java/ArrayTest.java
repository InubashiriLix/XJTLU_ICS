import java.util.Arrays;
import java.util.Scanner;
import java.util.stream.IntStream;

public class ArrayTest {

    public static <T> int LinearSearch(T[] arr, T key) {
        return IntStream.range(0, arr.length).filter(i -> arr[i].equals(key)).findFirst().orElse(-1);
    }

    public static <T extends Comparable<? super T>> int BinarySerach(T[] arr, T key) {
        // check whether the arrys is sorted by increasing order
        boolean sorted = java.util.stream.IntStream.range(0, arr.length - 1)
                .allMatch(i -> arr[i].compareTo(arr[i + 1]) <= 0);

        if (!sorted)
            throw new IllegalArgumentException("Array must be sorted in increasing order for binary search.");

        int left = 0, right = arr.length - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            int cmp = arr[mid].compareTo(key);
            if (cmp == 0) {
                return mid;
            } else if (cmp < 0) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return -1;
    }

    public static <T extends Comparable<? super T>> void SelectionSort(T[] arr) {
        for (int i = 0; i < arr.length - 1; i++) {
            int minIndex = i;
            for (int j = i + 1; j < arr.length; j++) {
                if (arr[j].compareTo(arr[minIndex]) < 0) {
                    minIndex = j;
                }
            }
            T temp = arr[i];
            arr[i] = arr[minIndex];
            arr[minIndex] = temp;
        }
    }

    public static <T> void printArray(T[] arr) {
        if (arr == null) {
            System.out.println("null");
            return;
        }
        boolean hasCusToString;
        try {
            hasCusToString = arr.getClass().getComponentType().getMethod("toString")
                    .getDeclaringClass() != Object.class;
        } catch (NoSuchMethodException e) {
            hasCusToString = false;
        }
        if (!hasCusToString) {
            System.out.println(java.util.Arrays.toString(arr));
            return;
        }

        for (T t : arr) {
            System.out.println(t);
        }
    }

    public static <T> void reverse(T[] arr) {
        T temp;
        for (int i = 0, j = arr.length - 1; i < arr.length / 2; i++, j--) {
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }

    public static <T> T[] inverseArray(T[] arr) {
        if (arr == null) {
            return null;
        }
        T[] inversedArray = Arrays.copyOf(arr, arr.length);
        for (int i = 0, j = arr.length - 1; i < arr.length; i++, j--) {
            inversedArray[i] = arr[j];
        }
        return inversedArray;

    }

    public static void main() {
        System.out.println("Array Test Start!");

        // shit ways to create array
        double[] shit1 = new double[10];
        double[] shit2 = new double[10];
        double shit3[] = new double[10]; // old way
        double[] nullshit = null;
        System.out.println(nullshit == null);

        // int[] arr = new int {1, 2}; // WARNING: invalid
        // int[5] arr = {1, 2, 3, 4 , 5}; // WARNING: invalid
        int[] arr = { 1, 2, 3, 4, 5 };
        // int[5] arr = {1, 2, 3, 4, 5}; // invalid
        //
        // NOTE: std way to initailize array
        double[] stdArr1 = new double[] { 1, 2, 3, 4, 5 };
        System.out.println(stdArr1.length);
        double[] stdArr2;
        stdArr2 = new double[] { 1.1, 1.2 };
        System.out.println(stdArr2.length);

        // NOTE: read 10 fucking double from terminal and store them into array
        boolean enableshit1 = false;
        if (enableshit1) {
            Scanner sc = new Scanner(System.in);
            int inputDoubleArraySize = 10;
            double[] inputDoubleArray = new double[inputDoubleArraySize];
            System.out.println("write 10 times");
            for (int i = 0; i < inputDoubleArraySize; i++) {
                inputDoubleArray[i] = sc.nextDouble();
                System.out.println("you have entered: " + inputDoubleArray[i]);
            }
        }

        // NOTE: Initailize Array With random values
        double[] randomArray = new double[10];
        for (int i = 0; i < randomArray.length; i++) {
            randomArray[i] = Math.random() * 100;
            System.out.println(randomArray[i]);
        }

        // NOTE: summing all values in the array
        double sum = 0;
        for (double shit : randomArray) {
            sum += shit;
        }
        System.out.println("SHIT SUM");
        System.out.println(sum);

        // NOTE: finding hte min
        double min = randomArray[0];
        for (int i = 1; i < randomArray.length; i++) {
            min = randomArray[i] < min ? randomArray[i] : min;
        }
        System.out.println(min);

        // NOTE: shuffling
        double[] arrayToBeShuffled = new double[] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        for (int i = 0; i < arrayToBeShuffled.length; i++) {
            int j = (int) (arrayToBeShuffled.length * Math.random());
            double temp = arrayToBeShuffled[i];
            arrayToBeShuffled[i] = arrayToBeShuffled[j];
            arrayToBeShuffled[j] = temp;
        }
        Arrays.stream(arrayToBeShuffled).forEach(i -> System.out.printf("%.2f ", i));
        System.out.println();

        // NOTE: shifting array right
        double[] arrayToBeShiftLeft = new double[] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        double[] result = IntStream.range(0, arrayToBeShiftLeft.length)
                .mapToDouble(i -> arrayToBeShiftLeft[Math.floorMod(i - 1, arrayToBeShiftLeft.length)]).toArray();
        IntStream.range(0, result.length).forEach(i -> System.out.printf("%.2f ", result[i]));
        System.out.println();

        // NOTE: shifting array left
        double[] arrayToBeShiftRight = new double[] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

        double[] result1 = IntStream.range(0, arrayToBeShiftRight.length)
                .mapToDouble(i -> arrayToBeShiftRight[Math.floorMod(i + 1, arrayToBeShiftRight.length)]).toArray();
        IntStream.range(0, result1.length).forEach(i -> System.out.printf("%.2f ", result1[i]));
        System.out.println();

        // NOTE: array in java is a reference, so when you assign an array to another
        // variable, they both point to the same array in memory
        double[] memArr1 = new double[] { 1, 2, 3, 4, 5 };
        double[] memArr2 = memArr1; // memArr2 points to the same
        System.out.println(memArr1 == memArr2); // true

        // NOTE: Copying an array using arraycopy
        int[] sourceArray = { 2, 3, 5, 1, 10 };
        int[] targetArray = new int[sourceArray.length];
        System.arraycopy(sourceArray, 0, targetArray, 0, sourceArray.length);
        IntStream.range(0, targetArray.length).forEach(i -> System.out.printf("%d ", targetArray[i]));
        System.out.println();

        // NOTE: passing arrays to method
        // NOTED THAT we use Integer instead of int because we want to test the
        // printArray method which takes an array of any type
        // java use pass by value to pass arguments to a method
        // - for a param of an array type, the value of the parameter contains a
        // refereence to an array; this refrence is passed by the method.
        // - Any changes to hte array that occur inside the method body will affect
        // origtinal array that was passed as the argument.
        // - Different from a parameter of a primitive type value where the actual value
        // is passed
        // - Changing the value of the local parameter insde the method does not affect
        // the value of the varible outside the method
        Integer[] inputArray = { 1, 2, 3, 4, 5 };
        printArray(inputArray);

        // NOTE: Anynonymous Array:
        printArray(new Integer[] { 1, 2, 3, 4, 5 });

        // NOTE: reverse the fucking arr
        Integer[] arrayToBeInversed = { 1, 2, 3, 4, 5 };
        Integer[] inversedArray = IntStream.range(0, arrayToBeInversed.length)
                .map(i -> arrayToBeInversed[arrayToBeInversed.length - 1 - i])
                .boxed().toArray(Integer[]::new);
        printArray(inversedArray);
        printArray(inverseArray(new Integer[] { 1, 2, 3, 4, 5 }));
        reverse(arrayToBeInversed);
        printArray(arrayToBeInversed);

        // NOTE: Linear Searching Arrays:
        Integer[] arrayToBeSerached = { 1, 2, 3, 4, 5 };
        System.out.println("Index of 3: " + LinearSearch(arrayToBeSerached, 3));
        System.out.println("Index of 7: " + LinearSearch(arrayToBeSerached, 7));

        // NOTE: Linear Searching Arrays:
        System.out.println("Index of 3: " + BinarySerach(arrayToBeSerached, 3)); // expected : 2
        System.out.println("Index of 8: " + BinarySerach(arrayToBeSerached, 8)); // expected : 2
        //
        // NOTE: Selection Sort:
        Integer[] toSort = { 5, 2, 4, 6, 1, 3 };
        SelectionSort(toSort);
        System.out.print("Sort result: ");
        printArray(toSort);
        // expected : 1 2 3 4 5 6
        //
        // NOTE: use array.sort to sort an array
        Integer[] toSortWithArraySort = { 5, 2, 4, 6, 1, 3 };
        Arrays.sort(toSortWithArraySort);
        System.out.print("Sort result with array.sort: ");
        printArray(toSortWithArraySort);

    }
}
