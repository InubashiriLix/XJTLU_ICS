import java.util.Comparator;
import java.util.Iterator;
import java.lang.UnsupportedOperationException;
import java.util.stream.IntStream;

public class ARDeque<T> implements Iterable {
    protected static final int DEFAULT_CAPACITY = 10;
    protected T[] items;
    protected int front;
    protected int size;

    @SuppressWarnings("unchecked")
    public ARDeque() {
        items = (T[]) new Object[DEFAULT_CAPACITY];
        front = 0;
        size = 0;
    }

    @SuppressWarnings("unchecked")
    public void ensureCapbility() {
        if (size == items.length) {
            var newitems = (T[]) new Object[items.length * 2];
            for (int i = 0; i < size; i++) {
                newitems[i] = items[Math.floorMod(front + i, items.length)];
            }
        }
    }

    public void addFirst(T element) {
        ensureCapbility();
        front = Math.floorMod(front - 1, items.length);
        items[front] = element;
        size++;
    }

    public void addLast(T element) {
        ensureCapbility();
        int lastIndex = Math.floorMod(front + size, items.length);
        items[lastIndex] = element;
        size++;
    }

    public T removeFirst() {
        var ret = items[front];
        front = Math.floorMod(this.front + 1, items.length);
        size--;
        return ret;

    }

    public T removeLast() {
        var ret = items[Math.floorMod(front + size - 1, items.length)];
        size--;
        return ret;
    }

    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("[ ");
        IntStream.range(0, size).forEach(i -> sb.append(items[Math.floorMod(front + i, items.length)]).append(" "));
        sb.append("]");
        return sb.toString();
    }

    public void printDeque() {
        System.out.println(this.toString());
    }

    public int size() {
        return size;
    }

    @Override
    public Iterator<T> iterator() {
        return new Iterator<T>() {
            private int index = 0;

            @Override
            public boolean hasNext() {
                return index < size;
            }

            @Override
            public T next() {
                return items[Math.floorMod(front + index++, items.length)];
            }
        };
    }
}
