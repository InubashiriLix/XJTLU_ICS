package IterableTest;

import java.util.List;
import java.util.ListIterator;
import java.util.Arrays;
import java.util.Collection;
import java.util.Iterator;
import java.util.NoSuchElementException;

public class Insertion implements List<Chronos>, Iterable<Chronos> {
    public static final int DEFAULT_CAPACITY = 10;
    private Chronos[] items;
    private int size;
    private int head;

    public Insertion() {
        items = new Chronos[DEFAULT_CAPACITY];
        Arrays.fill(items, null);
        size = 0;
        head = 0;
    }

    public Insertion(Chronos[] chronosArray) {
        items = new Chronos[chronosArray.length];
        for (int i = 0; i < chronosArray.length; i++) {
            items[i] = chronosArray[i];
        }
        size = chronosArray.length;
        head = 0;
    }

    private void ensureCapcity() {
        if (size >= items.length) {
            Chronos[] newItems = new Chronos[items.length * 2];
            for (int i = 0; i < items.length; i++) {
                int index = Math.floorMod(head + i, items.length);
                newItems[index] = items[index];
            }
            items = newItems;
        }
    }

    public Chronos get(int index) {
        if (index < 0 || index >= size) {
            throw new IndexOutOfBoundsException("Index: " + index + ", Size: " + size);
        }
        int actualIndex = Math.floorMod(head + index, items.length);
        return items[actualIndex];
    }

    public void addFirst(Chronos item) {
        ensureCapcity();
        head = Math.floorMod(head - 1, items.length);
        items[head] = item;
        size++;
    }

    public void addLast(Chronos item) {
        ensureCapcity();
        int tailIndex = Math.floorMod(head + size, items.length);
        items[tailIndex] = item;
        size++;
    }

    public Iterator<Chronos> iterator() {
        return new Iterator<Chronos>() {
            private int currentIndex = 0;

            public boolean hasNext() {
                return currentIndex < size;
            }

            public Chronos next() {
                if (!hasNext()) {
                    throw new NoSuchElementException();
                }
                var ret = get(currentIndex);
                currentIndex++;
                return ret;
            }
        };
    }

    public int size() {
        return size;
    }

    public boolean isEmpty() {
        return size == 0;
    }

    public Object[] toArray() {
        Object[] array = new Object[size];
        for (int i = 0; i < size; i++) {
            array[i] = get(i);
        }
        return array;
    }

    public boolean add(Chronos item) {
        addLast(item);
        return true;
    }

    public boolean contains(Object var1) {
        if (!(var1 instanceof Chronos)) {
            return false;
        }
        Chronos target = (Chronos) var1;
        for (int i = 0; i < size; i++)
            if (get(i).equals(target))
                return true;

        return false;
    }

    public Chronos remove(int index) {
        if (index < 0 || index >= size)
            throw new IndexOutOfBoundsException("Index: " + index + ", Size: " + size);
        Chronos ret = get(index);
        // we move index + 1 to index.
        for (int i = index; i < size - 1; i++) {
            int from = Math.floorMod(head + i + 1, items.length);
            int to = Math.floorMod(head + i, items.length);
            items[to] = items[from];
        }
        return ret;
    }

    public boolean remove(Object o) {
        if (!(o instanceof Chronos)) {
            return false;
        }
        Chronos target = (Chronos) o;
        int i = 0;
        for (var item : this) {
            if (get(i).equals(target)) {
                return true;
            }
            i++;
        }
        // target not found
        return false;
    }

    public boolean containsAll(Collection<?> var1) {
        for (Object item : var1) {
            if (!contains(item)) {
                return false;
            }
        }
        return true;
    }

    public Chronos set(int index, Chronos element) {
        if (index < 0 || index >= size)
            throw new IndexOutOfBoundsException();
        int actualIndex = Math.floorMod(head + index, items.length);
        Chronos old = items[actualIndex];
        items[actualIndex] = element;
        return old;
    }

    public boolean addAll(Collection<? extends Chronos> var1) {
        for (Chronos item : var1) {
            addLast(item);
        }
        return true;
    }

    public boolean addAll(int var1, Collection<? extends Chronos> var2) {
        throw new UnsupportedOperationException("addAll at index is not supported");
    }

    public boolean retainAll(Collection<?> var1) {
        throw new UnsupportedOperationException("retainAll is not supported");
    }

    public ListIterator<Chronos> listIterator() {
        return new ListIterator<Chronos>() {
            private int cursor = 0;

            public boolean hasNext() {
                return cursor < size;
            }

            public Chronos next() {
                return get(cursor++);
            }

            public boolean hasPrevious() {
                return cursor > 0;
            }

            public Chronos previous() {
                return get(--cursor);
            }

            public int nextIndex() {
                return cursor;
            }

            public int previousIndex() {
                return cursor - 1;
            }

            public void set(Chronos e) {
                Insertion.this.set(cursor - 1, e);
            }

            public void remove() {
                throw new UnsupportedOperationException();
            }

            public void add(Chronos e) {
                throw new UnsupportedOperationException();
            }
        };
    }

    public void add(int index, Chronos element) {
        if (index < 0 || index > size)
            throw new IndexOutOfBoundsException();
        ensureCapcity();
        for (int i = size; i > index; i--) {
            int from = Math.floorMod(head + i - 1, items.length);
            int to = Math.floorMod(head + i, items.length);
            items[to] = items[from];
        }
        int actualIndex = Math.floorMod(head + index, items.length);
        items[actualIndex] = element;
        size++;
    }

    public void clear() {
        for (int i = 0; i < items.length; i++)
            items[i] = null;
        size = 0;
        head = 0;
    }

    public int indexOf(Object o) {
        if (!(o instanceof Chronos))
            return -1;
        for (int i = 0; i < size; i++)
            if (get(i).equals(o))
                return i;
        return -1;
    }

    public int lastIndexOf(Object o) {
        if (!(o instanceof Chronos))
            return -1;
        for (int i = size - 1; i >= 0; i--)
            if (get(i).equals(o))
                return i;
        return -1;
    }

    public ListIterator<Chronos> listIterator(int index) {
        return new ListIterator<Chronos>() {
            private int cursor = index;

            public boolean hasNext() {
                return cursor < size;
            }

            public Chronos next() {
                return get(cursor++);
            }

            public boolean hasPrevious() {
                return cursor > 0;
            }

            public Chronos previous() {
                return get(--cursor);
            }

            public int nextIndex() {
                return cursor;
            }

            public int previousIndex() {
                return cursor - 1;
            }

            public void set(Chronos e) {
                Insertion.this.set(cursor - 1, e);
            }

            public void remove() {
                throw new UnsupportedOperationException();
            }

            public void add(Chronos e) {
                throw new UnsupportedOperationException();
            }
        };
    }

    public boolean removeAll(Collection<?> c) {
        boolean changed = false;
        for (Object o : c)
            changed |= remove(o);
        return changed;
    }

    public List<Chronos> subList(int fromIndex, int toIndex) {
        // 简易实现：返回不可变的 ArrayList 视图
        Chronos[] sub = new Chronos[toIndex - fromIndex];
        for (int i = fromIndex; i < toIndex; i++)
            sub[i - fromIndex] = get(i);
        return Arrays.asList(sub);
    }

    @SuppressWarnings("unchecked")
    public <T> T[] toArray(T[] a) {
        if (a.length < size)
            a = (T[]) java.lang.reflect.Array.newInstance(a.getClass().getComponentType(), size);
        for (int i = 0; i < size; i++)
            a[i] = (T) get(i);
        if (a.length > size)
            a[size] = null;
        return a;
    }
}
