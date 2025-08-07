package Queue;

import java.util.AbstractQueue;
import java.util.Iterator;

public class LinkedQueue<E> extends AbstractQueue<E> {
    private static class Node<E> {
        public E data;
        public Node<E> next;

        public int size() {
            int cnt = 0;
            var temp = this;
            while (temp.data != null) {
                cnt++;
                temp = temp.next;
            }
            return cnt;
        }

        public Node(E data, Node<E> next) {
            this.data = data;
            this.next = next;
        }
    }

    private Node<E> front = null;
    private Node<E> back = null;

    public LinkedQueue() {

    }

    public int size() {
        if (front == null)
            return 0;
        return front.size();
    }

    public boolean isEmpty() {
        return front == null;
    }

    public E peek() {
        if (front == null) {
            return null;
        } else
            return front.data;

    }

    public E poll() {
        if (front == null)
            return null;
        E val = front.data;
        front = front.next;
        if (front == null) {
            back = null;
        }
        return val;
    }

    public boolean offer(E item) {
        if (item == null)
            return false;
        // @SuppressWarnings("unchecked")
        if (front == null) {
            this.back = new Node<E>(item, null);
            front = back;
        } else {
            this.back.next = (new Node<E>(item, null));
            back = back.next;
        }
        return true;
    }

    public Iterator<E> iterator() {
        return new Iterator<E>() {
            private Node<E> cursor = front;

            public E next() {
                E val = cursor.data;
                cursor = cursor.next;
                return val;
            }

            public boolean hasNext() {
                return cursor != null;
            }
        };
    }
}
