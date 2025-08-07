package Bag;

import java.util.Map;
import java.util.NoSuchElementException;
import java.util.HashMap;
import java.util.Set;
import java.lang.Object;
import java.util.Iterator;

public class HashBag<E> implements Iterable<E> {
    private final Map<E, Integer> freq = new HashMap<>();
    private int total = 0;

    public int count(E item) {
        return freq.getOrDefault(item, 0);
    }

    public int size() {
        return total;
    }

    public boolean isEmpty() {
        return total == 0;
    }

    public boolean contains(Object obj) {
        return freq.containsKey(obj);
    }

    public Set<E> uniqueSet() {
        return freq.keySet();
    }

    public boolean add(E item) {
        return add(item, 1);
    }

    public boolean add(E item, int n) {
        if (n <= 0)
            return false;
        freq.merge(item, n, Integer::sum);
        total += n;
        return true;
    }

    public boolean remove(Object obj) {
        return remove(obj, 1) > 0;
    }

    public int remove(Object obj, int n) {
        if (n <= 0)
            return 0;
        @SuppressWarnings("unchecked")
        E item = (E) obj;
        Integer old = freq.get(item);
        if (old == null)
            return 0;
        int removed = Math.min(n, old);
        int newCnt = old - removed;
        if (newCnt == 0)
            freq.remove(item);
        else
            freq.put(item, newCnt);

        total -= removed;
        return removed;
    }

    public void clear() {
        freq.clear();
        total = 0;
    }

    @Override
    public Iterator<E> iterator() {
        return new BagIterator();
    }

    private class BagIterator implements Iterator<E> {
        private final Iterator<Map.Entry<E, Integer>> entryIt = freq.entrySet().iterator();
        private Map.Entry<E, Integer> currentEntry = null;
        private int remaining = 0;
        private boolean canRemove = false;

        @Override
        public boolean hasNext() {
            return remaining > 0 || entryIt.hasNext();
        }

        @Override
        public E next() {
            if (!hasNext())
                throw new NoSuchElementException();
            if (remaining == 0) {
                currentEntry = entryIt.next();
                remaining = currentEntry.getValue();
            }
            remaining--;
            canRemove = true;
            return currentEntry.getKey();
        }

        public void remove() {
            if (!canRemove)
                throw new IllegalStateException("next() must be called before remove()");
            HashBag.this.remove(currentEntry.getKey());
            canRemove = false;
        }
    }

    public static void demo() {
        HashBag<String> bag = new HashBag<>();

        bag.add("apple");
        bag.add("apple");
        bag.add("pear", 3);

        System.out.println("count(apple) = " + bag.count("apple")); // 2
        System.out.println("total size   = " + bag.size()); // 5

        bag.remove("pear", 2);
        System.out.println("after rm pear x2, size = " + bag.size());// 3

        System.out.println("\\n迭代输出：");
        for (String s : bag)
            System.out.println(s);

        System.out.println("\\nuniqueSet: " + bag.uniqueSet());
    }
}
