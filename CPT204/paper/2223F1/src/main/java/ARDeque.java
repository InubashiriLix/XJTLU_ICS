import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.Objects;
import java.util.stream.IntStream;

public class ARDeque<T> implements Iterable<T>, Cloneable {
    private T[] items;
    private int front;
    private int size;

    @SuppressWarnings("unchecked")
    public ARDeque() {
        items = (T[]) new Object[8];
        front = 0;
        size = 0;
    }

    public ARDeque(T[] initialItems) {
        items = (T[]) new Object[Math.max(8, initialItems.length * 2)];
        System.arraycopy(initialItems, 0, items, 0, initialItems.length);
        front = 0;
        size = initialItems.length;
    }

    public int size() {
        return size;
    }

    public boolean isEmpty() {
        return size == 0;
    }

    public void addFirst(T item) {
        ensureCapacity();

        front = Math.floorMod(front - 1, items.length);
        items[front] = item;
        size++;
    }

    public boolean contains(T item) {
        for (T i : this) {
            if (i.equals(item)) {
                return true;
            }
        }
        return false;

    }

    public void addLast(T item) {
        ensureCapacity();

        int lastIndex = Math.floorMod(front + size, items.length);
        items[lastIndex] = item;
        size++;
    }

    public T removeFirst() {
        if (isEmpty()) {
            throw new NoSuchElementException("Deque is empty");
        }

        T item = items[front];
        items[front] = null;
        front = Math.floorMod(front + 1, items.length);
        size--;

        shrinkIfNeeded();
        return item;
    }

    public T removeLast() {
        if (isEmpty()) {
            throw new NoSuchElementException("Deque is empty");
        }

        int lastIndex = Math.floorMod(front + size - 1, items.length);
        T item = items[lastIndex];
        items[lastIndex] = null;
        size--;

        shrinkIfNeeded();
        return item;
    }

    public T getFirst() {
        if (isEmpty()) {
            throw new NoSuchElementException("Deque is empty");
        }

        return items[front];
    }

    public T getLast() {
        if (isEmpty()) {
            throw new NoSuchElementException("Deque is empty");
        }

        int lastIndex = Math.floorMod(front + size - 1, items.length);
        return items[lastIndex];
    }

    public void printDeque() {
        for (T item : this) {
            System.out.print(item + " ");
        }
        System.out.println();
    }

    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("[ ");
        for (var item : this) {
            sb.append(item).append(" ");
        }
        sb.append(" ]");
        return sb.toString();
    }

    private void ensureCapacity() {
        if (size == items.length) {
            resize(items.length * 2);
        }
    }

    private void shrinkIfNeeded() {
        if (items.length > 8 && size <= items.length / 4) {
            resize(items.length / 2);
        }
    }

    @SuppressWarnings("unchecked")
    private void resize(int newCapacity) {
        T[] newItems = (T[]) new Object[newCapacity];

        for (int i = 0; i < size; i++) {
            newItems[i] = items[Math.floorMod(front + i, items.length)];
        }

        items = newItems;
        front = 0;
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
                if (!hasNext()) {
                    throw new NoSuchElementException();
                }

                T item = items[Math.floorMod(front + index, items.length)];
                index++;
                return item;
            }
        };
    }

    @Override
    public boolean equals(Object that) {
        if (this == that) {
            return true;
            // if smae object (reference type), then they are equal
        }

        // test if object is subclass or same class of ARDeque
        if (!(that instanceof ARDeque)) {
            return false;
        }

        // then compare the size of two deques and compare the item in the deque
        ARDeque<?> other = (ARDeque<?>) that;
        if (this.size != other.size) {
            return false;
        }

        // if two queues only have one or less difference in position, then they are
        // equal
        return IntStream.range(0, this.size)
                .filter(i -> !Objects.equals(
                        this.items[Math.floorMod(this.front + i, this.items.length)],
                        other.items[Math.floorMod(other.front + i, other.items.length)]))
                .limit(2)
                .count() <= 1;
    }

    @Override
    public ARDeque<T> clone() {
        ARDeque<T> cloned = new ARDeque<T>();
        for (T item : this) {
            cloned.addLast(item);
        }
        return cloned;
    }
}
