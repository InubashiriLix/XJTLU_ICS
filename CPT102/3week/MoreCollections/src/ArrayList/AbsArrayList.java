package ArrayList;

import java.util.*;

/**
 * An abstract skeletal implementation of a List backed by an array.
 * Subclasses only need to implement the five "primitive" methods:
 * size(), get(int), set(int, T), add(int, T), remove(int).
 * All other List operations are defined in terms of these.
 * 
 * @param <T> element type
 */
public abstract class AbsArrayList<T> implements List<T>, RandomAccess {

    /**
     * {@inheritDoc}
     * 
     * @return the number of elements
     */
    @Override
    public abstract int size();

    /**
     * {@inheritDoc}
     * 
     * @param index index to retrieve
     * @return element at index
     * @throws IndexOutOfBoundsException if index < 0 || index >= size()
     */
    @Override
    public abstract T get(int index);

    /**
     * Primitive: set element at index, return old value
     */
    // @Override
    public T set(int index, T element) {
        throw new UnsupportedOperationException();
    }

    /**
     * Primitive: insert element at index
     */
    // @Override
    public void add(int index, T element) {
        throw new UnsupportedOperationException();
    }

    /**
     * Primitive: remove element at index, return old value
     */
    // @Override
    public T remove(int index) {
        throw new UnsupportedOperationException();
    }

    /* ---------- Optional operations implemented ---------- */

    @Override
    public boolean isEmpty() {
        return size() == 0;
    }

    @Override
    public boolean contains(Object o) {
        return indexOf(o) >= 0;
    }

    @Override
    public Iterator<T> iterator() {
        return listIterator(0);
    }

    // @Override
    public Object[] toArray() {
        Object[] a = new Object[size()];
        for (int i = 0; i < a.length; i++) {
            a[i] = get(i);
        }
        return a;
    }

    // @Override
    public <E> E[] toArray(E[] a) {
        int s = size();
        if (a.length < s) {
            // allocate new array of runtime type
            @SuppressWarnings("unchecked")
            E[] newArr = (E[]) java.lang.reflect.Array
                    .newInstance(a.getClass().getComponentType(), s);
            a = newArr;
        }
        for (int i = 0; i < s; i++) {
            @SuppressWarnings("unchecked")
            E e = (E) get(i);
            a[i] = e;
        }
        if (a.length > s) {
            a[s] = null; // end marker
        }
        return a;
    }

    @Override
    public boolean add(T e) {
        add(size(), e);
        return true;
    }

    @Override
    public boolean remove(Object o) {
        int idx = indexOf(o);
        if (idx >= 0) {
            remove(idx);
            return true;
        }
        return false;
    }

    // @Override
    // public boolean containsAll(Collection<?> c) {
    // for (Object e : c) {
    // if (!contains(e))
    // return false;
    // }
    // return true;
    // }

    // @Override
    // public boolean addAll(Collection<? extends T> c) {
    // return addAll(size(), c);
    // }

    // @Override
    // public boolean addAll(int index, Collection<? extends T> c) {
    // int i = index;
    // for (T e : c) {
    // add(i++, e);
    // }
    // return !c.isEmpty();
    // }

    // @Override
    // public boolean removeAll(Collection<?> c) {
    // boolean modified = false;
    // Iterator<T> it = iterator();
    // while (it.hasNext()) {
    // if (c.contains(it.next())) {
    // it.remove();
    // modified = true;
    // }
    // }
    // return modified;
    // }

    // @Override
    // public boolean retainAll(Collection<?> c) {
    // boolean modified = false;
    // Iterator<T> it = iterator();
    // while (it.hasNext()) {
    // if (!c.contains(it.next())) {
    // it.remove();
    // modified = true;
    // }
    // }
    // return modified;
    // }

    // @Override
    public void clear() {
        removeRange(0, size());
    }

    /**
     * Remove all elements in [fromIndex, toIndex).
     */
    protected void removeRange(int fromIndex, int toIndex) {
        for (int i = toIndex - 1; i >= fromIndex; i--) {
            remove(i);
        }
    }

    // @Override
    public int indexOf(Object o) {
        int s = size();
        if (o == null) {
            for (int i = 0; i < s; i++) {
                if (get(i) == null)
                    return i;
            }
        } else {
            for (int i = 0; i < s; i++) {
                if (o.equals(get(i)))
                    return i;
            }
        }
        return -1;
    }

    // @Override
    public int lastIndexOf(Object o) {
        for (int i = size() - 1; i >= 0; i--) {
            if (Objects.equals(o, get(i)))
                return i;
        }
        return -1;
    }

    // @Override
    public ListIterator<T> listIterator() {
        return listIterator(0);
    }

    // @Override
    public ListIterator<T> listIterator(int index) {
        if (index < 0 || index > size())
            throw new IndexOutOfBoundsException("Index: " + index);
        return new ListItr(index);
    }

    private class ListItr implements ListIterator<T> {
        int cursor; // index of next element
        int lastRet = -1; // index of last returned, -1 if none

        ListItr(int index) {
            this.cursor = index;
        }

        public boolean hasNext() {
            return cursor < size();
        }

        public T next() {
            int i = cursor;
            if (i >= size())
                throw new NoSuchElementException();
            cursor = i + 1;
            lastRet = i;
            return get(i);
        }

        public boolean hasPrevious() {
            return cursor > 0;
        }

        public T previous() {
            int i = cursor - 1;
            if (i < 0)
                throw new NoSuchElementException();
            cursor = i;
            lastRet = i;
            return get(i);
        }

        public int nextIndex() {
            return cursor;
        }

        public int previousIndex() {
            return cursor - 1;
        }

        public void remove() {
            if (lastRet < 0)
                throw new IllegalStateException();
            AbsArrayList.this.remove(lastRet);
            if (lastRet < cursor)
                cursor--;
            lastRet = -1;
        }

        public void set(T e) {
            if (lastRet < 0)
                throw new IllegalStateException();
            AbsArrayList.this.set(lastRet, e);
        }

        public void add(T e) {
            AbsArrayList.this.add(cursor++, e);
            lastRet = -1;
        }
    }

    // @Override
    // public List<T> subList(int fromIndex, int toIndex) {
    // Objects.checkFromToIndex(fromIndex, toIndex, size());
    // List<T> sub = new ArrayList<>();
    // for (int i = fromIndex; i < toIndex; i++) {
    // sub.add(get(i));
    // }
    // return sub;
    // }

    @Override
    public boolean equals(Object o) {
        if (o == this)
            return true;
        if (!(o instanceof List))
            return false;
        List<?> other = (List<?>) o;
        if (other.size() != size())
            return false;
        Iterator<T> it1 = iterator();
        Iterator<?> it2 = other.iterator();
        while (it1.hasNext() && it2.hasNext()) {
            T e1 = it1.next();
            Object e2 = it2.next();
            if (!Objects.equals(e1, e2))
                return false;
        }
        return !(it1.hasNext() || it2.hasNext());
    }

    @Override
    public int hashCode() {
        int hash = 1;
        for (T e : this) {
            hash = 31 * hash + (e == null ? 0 : e.hashCode());
        }
        return hash;
    }
}
