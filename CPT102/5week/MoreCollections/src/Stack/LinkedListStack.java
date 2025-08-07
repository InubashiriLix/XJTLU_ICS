package Stack;

import LinkedList.LinkedList;
import java.util.Iterator;

public class LinkedListStack<E> extends LinkedList<E> {
    /*
     * the method below has been implemented in the LinkedList
     * int size();
     * boolean isEmpty();
     * E get(int index);
     * void add(int index, E item);
     * void add(item);
     * E set(int index, E item);
     * boolean remove(Object item);
     * E remove(int index);
     * Iterator<E> iterator();
     */

    public boolean push(E item) {
        return super.add(item);
    }

    public E peek() {
        return get(super.size() - 1);
    }

    public E pop() {
        return super.remove(super.size() - 1);
    }

    public LinkedListStack() {

    }

    public LinkedListStack(E item) {
        super.add(super.size(), item);
    }
}
