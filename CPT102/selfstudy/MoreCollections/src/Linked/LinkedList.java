package Linked;

import java.util.Iterator;
import ArrayList.AbsArrayList;

public class LinkedList<E> extends AbsArrayList<E> {

    private class Node<E> {
        public E data;
        public Node<E> next;

        public Node(E value, Node<E> next) {
            this.data = value;
            this.next = next;
        }
    }

    public final Node<E> list = new Node<E>(null, null);

    private int size = 0;

    public int size() {
        return size;
    }

    public E get(int index) {
        if (index >= size || index < 0)
            throw new IndexOutOfBoundsException();
        int cnt = -1;
        for (var rest = list.next; rest != null; rest = rest.next) {
            cnt++;
            if (cnt == index)
                return rest.data;
        }
        return null;
    }

    public E set(int index, E item) {
        if (index >= size || index < 0)
            throw new IndexOutOfBoundsException();
        int cnt = -1;
        for (Node<E> rest = list.next; rest != null; rest = rest.next) {
            cnt++;
            if (cnt == index) {
                var old = rest.data;
                rest.data = item;
                return old;
            }
        }
        return null;
    }

    public void add(int index, E item) {
        if (index < 0 || index > size)
            throw new IndexOutOfBoundsException();
        int cnt = -2;
        for (var rest = list; rest != null; rest = rest.next) {
            cnt++;
            if (cnt == index - 1) {
                rest.next = (new Node<E>(item, rest.next));
                size++;
                return;
            }
        }
    }

    public boolean add(E item) {
        var rest = list;
        while (rest.next != null) {
            rest = rest.next;
        }
        rest.next = (new Node<E>(item, null));
        size++;
        return true;
    }

    public boolean remove(Object item) {
        var rest = list;
        while (rest.next != null) {
            if (rest.next.data != null) {
                if (rest.next.data.equals(item)) {
                    rest.next = (rest.next.next);
                    size--;
                    return true;
                }
            } else {
                if (item == null) {
                    rest.next = (rest.next.next);
                    size--;
                    return true;
                }
            }
            rest = rest.next;
        }
        return false;
    }

    public E remove(int index) {
        if (index < 0 || index >= size)
            throw new IndexOutOfBoundsException();
        int cnt = -1;
        var rest = list;
        while (rest.next != null) {
            cnt++;
            if (cnt == index) {
                var rtn = rest.next.data;
                rest.next = (rest.next.next);
                size--;
                return rtn;
            }
            rest = rest.next;
        }
        return null;
    }

    public Iterator<E> iterator() {
        return new Iterator<>() {
            private Node<E> currentNode = list.next;

            @Override
            public boolean hasNext() {
                return currentNode != null;
            }

            @Override
            public E next() {
                if (currentNode == null)
                    throw new IndexOutOfBoundsException();
                var temp = currentNode.data;
                currentNode = currentNode.next;
                return temp;
            }
        };
    }

    public LinkedList() {
        // WARNING: this class only works for linkedNodes with dummy head

    }
}
