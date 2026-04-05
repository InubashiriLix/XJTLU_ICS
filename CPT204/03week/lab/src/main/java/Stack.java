import java.lang.reflect.Method;
import java.util.ArrayList;

public class Stack<E> implements Cloneable {
    private java.util.ArrayList<E> list = new java.util.ArrayList<>();

    public void push(E o) {
        list.add(o);
    }

    public Object pop() {
        E o = list.get(getSize() - 1);
        list.remove(getSize() - 1);
        return o;
    }

    public E peek() {
        return list.get(getSize() - 1);
    }

    public int search(E o) {
        return list.lastIndexOf(o);
    }

    public boolean isEmpty() {
        return list.isEmpty();
    }

    public int getSize() {
        return list.size();
    }

    public String toString() {
        return "stack: " + list.toString();
    }

    @Override
    public Stack<E> clone() {
        try {
            Stack<E> cloned = new Stack<E>();
            cloned.list = (ArrayList<E>) list.clone();
            return cloned;
        } catch (Exception e) {
            throw new AssertionError(e);
        }
    }
}
