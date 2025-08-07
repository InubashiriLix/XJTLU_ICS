package ArrayList;

import java.util.Iterator;
import java.util.ListIterator;
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

    public boolean add(int index, T element) {
        if (index < 0 || index > size) {
            throw new IndexOutOfBoundsException("Index: " + index + ", Size: " + size);
        }
        if (size == elements.length) {
            Object[] newArray = new Object[elements.length * 2];
            System.arraycopy(elements, 0, newArray, 0, elements.length);
            this.elements = newArray;
        }
        System.arraycopy(elements, index, elements, index + 1, size - index);
        elements[index] = element;
        size++;
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

    public void clear() {
        for (int i = 0; i < this.size; i++) {
            elements[i] = null;
        }
        this.size = 0;
    }

    public boolean set(int index, T element) {
        if (index < 0 || index >= this.size) {
            throw new IndexOutOfBoundsException("Index: " + index + ", Size: " + this.size);
        }
        elements[index] = element;
        return true;
    }

    public int indexOf(T element) {
        for (int i = 0; i < this.size; i++) {
            if (this.elements[i].equals(element)) {
                return i;
            }
        }
        return -1;
    }

    public void remove(int index) {
        if (index < 0 || index >= size) {
            throw new IndexOutOfBoundsException("Index: " + index + ", Size: " + size);
        }
        for (int i = index; i < size - 1; i++) {
            elements[i] = elements[i + 1];
        }
        elements[size - 1] = null; // Clear the last element
        size--;
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
        for (int i = 0; i < size; i++) {
            if (o == null ? elements[i] == null : o.equals(elements[i])) {
                int numMoved = size - i - 1;
                if (numMoved > 0) {
                    System.arraycopy(elements, i + 1, elements, i, numMoved);
                }
                elements[--size] = null;
                return true;
            }
        }
        return false;
    }

    public int lastIndexof(Object o) {
        int rtnIdx = -1;
        for (int i = 0; i < this.size; i++) {
            if (this.elements[i].equals(o)) {
                rtnIdx = i;
            }
        }
        return rtnIdx;
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
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        List<?> other = (List<?>) o;
        if (this.size() != other.size()) {
            return false;
        }
        for (int i = 0; i < this.size(); i++) {
            if (!this.get(i).equals(other.get(i))) {
                return false;
            }
        }
        return true;
    }

    public ListIterator<T> listIterator() {
        return new ListIterator<T>() {
            private int currentIndex = -1;

            @Override
            public boolean hasNext() {
                return currentIndex + 1 < size;
            }

            @Override
            public T next() {
                if (!hasNext()) {
                    throw new NoSuchElementException();
                }
                // @SuppressWarnings("unchecked")
                return (T) elements[++currentIndex];
            }

            @Override
            public boolean hasPrevious() {
                return currentIndex >= 0;
            }

            @Override
            public T previous() {
                if (!hasPrevious()) {
                    throw new NoSuchElementException();
                }
                // @SuppressWarnings("unchecked")
                return (T) elements[currentIndex--];
            }

            @Override
            public int nextIndex() {
                return currentIndex + 1;
            }

            @Override
            public int previousIndex() {
                return currentIndex;
            }

            @Override
            public void remove() {
                if (currentIndex < 0 || currentIndex >= size) {
                    throw new IllegalStateException();
                }
                ArrayList.this.remove(currentIndex);
                currentIndex--;
            }

            @Override
            public void set(T e) {
                if (currentIndex < 0 || currentIndex >= size) {
                    throw new IllegalStateException();
                }
                elements[currentIndex] = e;
            }

            @Override
            public void add(T e) {
                ArrayList.this.add(currentIndex + 1, e);
                currentIndex++;
            }
        };
    }

    public int hashCode() {
        int result = 1;
        for (var element : this.elements) {
            result = 31 * result + (element == null ? 0 : element.hashCode());
        }
        return result;
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
