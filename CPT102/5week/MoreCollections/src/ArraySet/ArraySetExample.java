package ArraySet;

import javax.swing.text.html.HTMLDocument.RunElement;

import ArraySet.ArraySet;

public class ArraySetExample {
    public ArraySetExample() {
        ArraySet<String> set = new ArraySet<>();
        set.add("test1");
        set.add("test2");
        set.add("test3");
        System.out.println("add tests done");

        try {
            set.add("test1");
        } catch (RuntimeException e) {
            System.out.println("the duplicated element has been handled");
        }

        set.add(0, "test0");

        // iterate the arraySet
        for (var ele : set) {
            System.out.println(ele);
        }

    }

}
