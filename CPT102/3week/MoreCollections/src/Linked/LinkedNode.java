package Linked;

public class LinkedNode<T> {

    private T data;
    private LinkedNode<T> next;

    public T get() {
        return data;
    }

    public void set(T item) {
        this.data = item;
    }

    public LinkedNode<T> next() {
        return next;
    }

    public void setNext(LinkedNode<T> next) {
        this.next = next;
    }

    public LinkedNode(T data, LinkedNode<T> next) {
        this.data = data;
        this.next = next;
    }

}
