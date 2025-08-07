package ArrayList;

import java.util.Iterator;

public class ArrayListAbs<T> extends AbsArrayList<T> {

    private T[] data;
    private int count;
    private static int INITIAL_CAPACITY = 16;
    private int sizeArray = INITIAL_CAPACITY;

    public ArrayListAbs() {
        // @SuppressWarnings("unchecked")
        this.data = (T[]) new Object[INITIAL_CAPACITY];
    }

    @Override
    public int size() {
        return count;
    }

    @Override
    public T get(int index) {
        if (index < 0 || index >= count) {
            throw new IndexOutOfBoundsException("index out of boundary");
        }
        return data[index];
    }

    @Override
    public T set(int index, T element) {
        if (index < 0 || index >= count) {
            throw new IndexOutOfBoundsException("index out of boundary");
        }
        data[index] = element;
        return data[index];
    }

    @Override
    public void add(int index, T element) {
        if (count > sizeArray) {
            throw new IndexOutOfBoundsException("detected data boundary");
        }
        if (count == sizeArray) {
            sizeArray = sizeArray + (sizeArray >> 1);
            T[] newArray = (T[]) new Object[sizeArray];
            System.arraycopy(data, 0, newArray, 0, count);
            data = newArray;
        }

        if (index < count) {
            System.arraycopy(data, index, data, index + 1, count - index);
        }

        data[index] = element;
        count++;
    }

    @Override
    public T remove(int index) {
        if (index < 0 || index >= count) {
            throw new IndexOutOfBoundsException("Index: " + index + ", Size: " + count);
        }
        T removedElement = data[index];

        int numMoved = count - index - 1;
        if (numMoved > 0) {
            System.arraycopy(data, index + 1, data, index, numMoved);
        }
        data[--count] = null;

        return removedElement;
    }

    // @Override
    // public Iterator<T> iterator() {
    // return new IteratorArrayList<T>(this);
    // }
}
