import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;

public class Main {
    public static void main(String[] args) {
        // ===================================================================================
        // System.out.println("Lab 4.1");
        // ArrayList<Integer> list1 = new ArrayList<>(List.of(1, 2, 3, 4, 5));
        // ArrayList<Double> list2 = new ArrayList<>(List.of(1.0, 2.0, 3.0, 4.0, 5.0));
        // ArrayList<Float> list3 = new ArrayList<Float>(List.of(1.0f, 2.0f, 3.0f, 4.0f,
        // 5.0f));
        // System.out.println(reverse(list1));
        // System.out.println(reverse(list2));
        // System.out.println(reverse(list3));
        // ===================================================================================
        // System.out.println("Lab 4.2");
        // ArrayList<Integer> list1 = new ArrayList<>(List.of(1, 2, 3, 4, 5));
        // ArrayList<Double> list2 = new ArrayList<>(List.of(1.0, 2.0, 3.0, 4.0, 5.0));
        // ArrayList<Float> list3 = new ArrayList<Float>(List.of(1.0f, 2.0f, 3.0f, 4.0f,
        // 5.0f));
        // System.out.println(shuffle(list1));
        // System.out.println(shuffle(list2));
        // System.out.println(shuffle(list3));
        // ===================================================================================
        // System.out.println("Lab 4.3");
        // ArrayList<Integer> list1 = new ArrayList<>(List.of(1, 2, 3, 4, 5));
        // ArrayList<Double> list2 = new ArrayList<>(List.of(1.0, 2.0, 3.0, 4.0, 5.0));
        // ArrayList<Float> list3 = new ArrayList<Float>(List.of(1.0f, 2.0f, 3.0f, 4.0f,
        // 5.0f));
        // System.out.println(max(list1));
        // System.out.println(max(list2));
        // System.out.println(max(list3));
        // ===================================================================================
        // System.out.println("Lab 4.4");
        // Pair<Integer, String> pair1 = new Pair<Integer, String>(1, "shit");
        // System.out.println(pair1.first);
        // System.out.println(pair1.second);
        // ===================================================================================
        // System.out.println("Lab 4.5");
        // explanation:
        // ArrayList<Integer> is not a subtype of ArrayList<Object>.
        // Although Integer is a subclass of Object, generic types in Java are
        // invariant.
        // So the method print(ArrayList<Object> o) cannot accept an ArrayList<Integer>.
        // One correction is to change the method parameter to ArrayList<?>.
        // end;
        // ArrayList<Integer> shit = new ArrayList(List.of(1, 2, 3, 4, 5));
        // printArray(shit);
        // ===================================================================================
        // System.out.println("Lab 4.6");
        // ArrayList<Integer> source = new ArrayList<>();
        // source.add(1);
        // source.add(2);
        // source.add(3);
        //
        // ArrayList<Number> target = new ArrayList<>();
        // copy(source, target);
        // System.out.println(target); // System.out.println("Lab 4.6");
        // ===================================================================================
        // System.out.println("Lab 4.7");

        // 1. the main advantage of generic in java is
        // -- the main advantage of genrics in java is type safety and code reusability
        // Generic allow us to write one class or method that works with different
        // reference types, while reducing the needs for explicit casting and catching
        // type errors at compile time instead of runtime;
        //
        // 2. Why does Java require generic type arguments to be reference types rather
        // than primitive types?
        // Java generics work only with reference types because of type erasure.
        // During compilation, generic type information is erased, and primitive types
        // such as int or double cannot be used as type arguments in this mechanism.
        // Therefore, wrapper classes such as Integer and Double are used instead.

        // 3. What is the difference between ?, ? extends T, and ? super T?
        // ? means an unbounded wildcard: the type can be anything.
        // ? extends T means the type is T or a subclass of T.
        // It is useful when we only need to read elements as type T.
        // ? super T means the type is T or a superclass of T.
        // It is useful when we want to add objects of type T into the structure.
        //
        // 4. Why is GenericStack<Integer> not a subclass of GenericStack<Number>,
        // even though Integer is a subclass of Number?
        // This is because generic types in Java are invariant.
        // Although Integer is a subclass of Number, GenericStack<Integer> and
        // GenericStack<Number> are treated as two unrelated types.
        // Otherwise, type safety could be broken.

        // 5. What is type erasure, and what effect does it have at runtime?
        // Type erasure means that generic type information is removed by the compiler
        // after compilation.
        // For example, generic type parameters are replaced by Object or by their
        // bounds.
        // At runtime, the JVM does not know the actual generic type arguments such as
        // Integer or String.
        // This is why some operations with generics are restricted.

        // 6. Why is "T value = new T();" not allowed in Java?
        // This is not allowed because the actual type of T is unknown at compile time.
        // Due to type erasure, Java does not know what constructor T should use,
        // so it cannot create an object with new T().
    }

    public static <T> ArrayList<T> reverse(ArrayList<T> list) {
        ArrayList<T> reversed = new ArrayList<>(list);
        for (int i = 0, j = reversed.size() - 1; i < j; i++, j--) {
            T temp = reversed.get(i);
            list.set(i, list.get(j));
            list.set(j, temp);
        }
        return list;
    }

    public static <E> ArrayList<E> shuffle(ArrayList<E> list) {
        for (int i = 0; i < list.size(); i++)
            for (int j = 0; j < list.size(); j++)
                Collections.swap(list, i, j);
        return list;
    }

    public static <E extends Comparable<E>> E max(ArrayList<E> list) {
        E max = null;
        for (E element : list) {
            if (max == null || element.compareTo(max) > 0) {
                max = element;
            }
        }
        return max;
    }

    public static void printArray(ArrayList<?> list) {
        for (Object e : list)
            System.out.print(e + ", ");
        System.out.println();
    }

    public static <T> void copy(ArrayList<? extends T> source, ArrayList<? super T> target) {
        for (T e : source)
            target.add(e);
    }
}
