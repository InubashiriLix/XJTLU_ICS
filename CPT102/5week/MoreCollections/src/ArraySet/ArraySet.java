package ArraySet;

import java.rmi.server.ObjID;
import java.util.Iterator;

import ArrayList.AbsArrayList;

public class ArraySet<E> extends AbsArrayList<E> {
    private int size = 0;
    private int CAP = 16;
    private Object[] elements = new Object[CAP];

    @Override
    public int size() {
        return this.size;
    }

    @Override
    public E get(int index) {
        if (index < 0 || index >= size) {
            throw new IndexOutOfBoundsException("Index: " + index + ", Size: " + size);
        }
        return (E) elements[index];
    }

    @Override
    public E set(int index, E element) {
        if (index < 0 || index >= size) {
            throw new IndexOutOfBoundsException();
        }
        var old = elements[index];
        elements[index] = element;
        return (E) old;
    }

    public boolean contains(Object o) {
        for (int i = 0; i < size; i++) {
            if (elements[i].equals(o))
                return true;
        }
        return false;
    }

    public void ensureCapability() {
        if (size > elements.length)
            throw new OutOfMemoryError();
        if (size == elements.length) {
            var newElements = new Object[elements.length * 2];
            System.arraycopy(elements, 0, newElements, 0, elements.length);
            elements = newElements;
        }
    }

    @Override
    public void add(int index, E element) {
        ensureCapability();
        if (!contains(element)) {
            for (int i = size; i > index; i--) {
                elements[i] = elements[i - 1];
            }
            elements[index] = element;
            size++;
        } else {
            throw new RuntimeException("Element already exists: " + element);

        }
    }

    @Override
    public boolean add(E element) {
        add(size, element);
        return true;
    }

    @Override
    public E remove(int index) {
        if (index < 0 || index >= size) {
            throw new IndexOutOfBoundsException("Index: " + index + ", Size: " + size);
        }
        var old = elements[index];
        for (int i = index; i < elements.length - 1; i++) {
            elements[i] = elements[i + 1];
        }
        size--;
        return (E) old;
    }

    private Iterator<E> SetItr() {
        return new ListItr();
    }

    public boolean remove(Object O) {
        for (int i = 0; i < size; i++) {
            if (elements[i].equals(O)) {
                elements[i] = null;
                remove(i);
                return true;
            }
        }
        return false;
    }

    @Override
    public boolean isEmpty() {
        return size == 0;

    }

    public class ListItr implements Iterator<E> {
        private int cursor = 0;

        @Override
        public boolean hasNext() {
            return cursor < size;
        }

        @Override
        public E next() {
            var rtn = elements[cursor];
            cursor++;
            return (E) rtn;
        }
    }

    @Override
    public Iterator<E> iterator() {
        return new ListItr();
    }

    public ArraySet() {

    }
}
