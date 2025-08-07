package Stack;

import java.sql.Array;
import java.util.Iterator;
import ArrayList.ArrayList;

public class ArrayStack<T> implements Stack<T> {
    private ArrayList<T> elements;

    public ArrayStack() {
        elements = new ArrayList<T>();
    }

    @Override
    public boolean isEmpty() {
        return elements.isEmpty();
    }

    @Override
    public int size() {
        return elements.size();
    }

    @Override
    public boolean add(T value) {
        return elements.add(value);
    }

    @Override
    public T get(int index) {
        return elements.get(index);
    }

    @Override
    public boolean contains(Object o) {
        return elements.contains(o);
    }

    @Override
    public boolean remove(T element) {
        return elements.remove(elements);
    }

    @Override
    public Iterator<T> iterator() {
        return elements.iterator();
    }

    @Override
    public void push(T value) {
        add(value);
    }

    @Override
    public T pop() {
        var temp = this.elements.get(this.elements.size() - 1);
        elements.remove(temp);
        return temp;
    }

    @Override
    public T peek() {
        var temp = this.elements.get(this.elements.size() - 1);
        return temp;
    }
}
