package Linked;

import Linked.LinkedNode;

public class UsingExample<T> {

    public void insert(T item, int n, LinkedNode<T> list) {
        // WARNING: only dowable for dummy head
        int pos = 0;
        LinkedNode<T> rest = list;
        while (pos < n - 1) {
            pos++;
            rest = rest.next();
        }
        rest.setNext(new LinkedNode<T>(item, rest.next()));
    }

    // public void insert(T item, int n, LinkedNode<T> list) {
    // // WARNING: only doable for dummy head
    // if (n == 1)
    // list.setNext(new LinkedNode<T>(item, list.next()));
    // else
    // insert(item, n - 1, list.next());
    // }

    // recrusive remove method
    // public void remove(T item, LinkedNode<T> list) {
    // // WARNING: this method only can work for list with dummy head!
    // if (item == null)
    // return;
    // if (list.next().get().equals(item)) {
    // list.setNext(list.next().next());
    // } else {
    // remove(item, list.next());
    // }
    // }

    public void remove(T item, LinkedNode<T> list) {
        // WARNING: this method only can work for list with dummy head!
        if (item == null)
            return;
        for (var rest = list; rest != null; rest = rest.next()) {
            if (rest.next().get().equals(item)) {
                rest.setNext(rest.next().next());
            }
        }
    }

    public T lastValue(LinkedNode<T> list) {
        // WARNING: this method only can work for list with dummy head!
        LinkedNode<T> last = list;
        for (var rest = list; rest != null; rest = rest.next()) {
            last = rest;
        }
        return last.get();

    }

    public UsingExample() {
        // LinkedNode<String> colors = new LinkedNode<>("red", null);
        // colors.setNext(new LinkedNode<>("blue", null));
        // colors = new LinkedNode<String>("green", colors);
        // another way to setup
        // from the end to the front ptr, the colors is the ptr
        var colors = new LinkedNode<String>("blue", null);
        colors = new LinkedNode<String>("red", colors);
        colors = new LinkedNode<String>("green", colors);

        System.out.format("1st, %s\n", colors.get());
        System.out.format("2nd, %s\n", colors.next().get());
        System.out.format("3rd, %s\n", colors.next().next().get());

        // green -> red -> blue

        // delete the red
        // green -> blue
        colors.setNext(colors.next().next());
        System.out.format("1st, %s\n", colors.get());
        System.out.format("2nd, %s\n", colors.next().get());

        // insert the node red between the green and blue
        var red = new LinkedNode<String>("red", colors.next());
        colors.setNext(red);

        // insert the new node called start at the beginging of the LinkedNodes
        colors = new LinkedNode<String>("start", colors);

        // insert thew new node called end at the end of the LinkedNodes
        // find the last node (not null)
        var temp_ptr1 = colors;
        var last = colors;
        while (temp_ptr1 != null) {
            last = temp_ptr1;
            temp_ptr1 = temp_ptr1.next();
        }
        // update the node
        last.setNext(new LinkedNode<String>("end", null));

        // iterate the linked nodes
        // should be like
        // start -> green -> red -> blue -> end
        var temp_ptr = colors;
        while (temp_ptr != null) {
            System.out.format("%s ", temp_ptr.get());
            temp_ptr = temp_ptr.next();
        }

        // copying the node list
        var temp_ptr_copy = new LinkedNode<String>(null, null);
        var copy_head = temp_ptr_copy;
        var temp_ptr2 = colors;
        while (temp_ptr2 != null) {
            temp_ptr_copy.setNext(new LinkedNode<String>(temp_ptr2.get(), null));
            temp_ptr_copy = temp_ptr_copy.next();
            temp_ptr2 = temp_ptr2.next();
        }
        copy_head = copy_head.next(); // skip the first node which is null

        // show the copy node list
        System.out.println("\ncopy show");
        // var shit_ptr = copy_head;
        // while (shit_ptr != null) {
        // System.out.format("%s ", shit_ptr.get());
        // shit_ptr = shit_ptr.next();
        // }
        // another way to iterate the linked notdes
        for (var shit_ptr = copy_head; shit_ptr != null; shit_ptr = shit_ptr.next()) {
            System.out.format("%s ", shit_ptr.get());
        }
    }
}
