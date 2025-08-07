package ArrayList;

import java.util.Iterator;
import java.util.NoSuchElementException;

import ArrayList.Collection;
import ArrayList.List;

public class ArrayList<T> implements List<T> {
    private Object[] elements;
    private int size;

    public ArrayList() {
        this.elements = new Object[10];
        this.size = 0;
    }

    @Override
    public boolean add(T value) {
        if (size == elements.length) {
            Object[] newArray = new Object[elements.length * 2];
            System.arraycopy(elements, 0, newArray, 0, elements.length);
            this.elements = newArray;
        }
        elements[size++] = value;
        return true;
    }

    @Override
    public T get(int index) {
        if (index < 0 || index >= this.size) {
            throw new IndexOutOfBoundsException("Index: " + index + ", Size: " + this.size);
        } else {
            @SuppressWarnings("unchecked")
            T result = (T) this.elements[index];
            return result;
        }

    }

    @Override
    public boolean contains(Object o) {
        for (int i = 0; i < this.size; i++) {
            if (o == null) {
                if (elements[i] == null) {
                    return true;
                }
            } else if (o.equals(elements[i])) {
                return true;
            }
        }
        return false;
    }

    @Override
    public boolean remove(Object o) {
        if (contains(o)) {
            for (int i = 0; i < this.size; i++) {
                if (elements[i].equals(o)) {
                    for (int j = i; j < this.size; j++) {
                        elements[j] = elements[j + 1];
                    }
                    elements[size - 1] = null;
                    return true;
                }
            }
        }
        return false;

    }

    @Override
    public Iterator<T> iterator() {
        return new Iterator<T>() {
            private int currentIndex = 0;

            @Override
            public boolean hasNext() {
                return currentIndex < size;

            }

            @Override
            public T next() {
                if (!hasNext()) {
                    throw new NoSuchElementException();
                }
                @SuppressWarnings("unchecked")
                T result = (T) elements[currentIndex++];
                return result;
            }
        };
    }

    @Override
    public int size() {
        return this.size;

    }

    @Override
    public boolean isEmpty() {
        return size == 0;
    }
}
