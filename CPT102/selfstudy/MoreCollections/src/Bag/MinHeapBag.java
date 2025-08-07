package Bag;

import java.util.HashMap;
import java.util.Map;
import java.util.ArrayList;
import java.util.List;

public class MinHeapBag<E extends Comparable<E>> {
    private final Map<E, Integer> cnt = new HashMap<>();
    private final List<E> heap = new ArrayList<>();
    private final Map<E, Integer> idx = new HashMap<>();
    private int total = 0;

    public void add(E e) {
        add(e, 1);
    }

    public void add(E e, int n) {
        if (n <= 0)
            return;
        int c = cnt.getOrDefault(e, 0) + n;
        cnt.put(e, c);
        total += n;
        if (c == n) { // 新元素
            heap.add(e);
            idx.put(e, heap.size() - 1);
            siftUp(heap.size() - 1);
        }
    }

    public int remove(E e, int n) {
        Integer cObj = cnt.get(e);
        if (cObj == null || n <= 0)
            return 0;
        int removed = Math.min(n, cObj);
        int newCnt = cObj - removed;
        total -= removed;
        if (newCnt == 0) {
            cnt.remove(e);
            int pos = idx.remove(e);
            removeAt(pos);
        } else {
            cnt.put(e, newCnt);
        }
        return removed;
    }

    public E peekMin() {
        return heap.isEmpty() ? null : heap.get(0);
    }

    public E pollMin() {
        if (heap.isEmpty())
            return null;
        E min = heap.get(0);
        remove(min, 1);
        return min;
    }

    public int count(E e) {
        return cnt.getOrDefault(e, 0);
    }

    public int size() {
        return total;
    }

    // heap helpers
    private void siftUp(int i) {
        while (i > 0) {
            int p = (i - 1) >> 1;
            if (heap.get(i).compareTo(heap.get(p)) >= 0)
                break;
            swap(i, p);
            i = p;
        }
    }

    private void siftDown(int i) {
        int n = heap.size();
        while (true) {
            int l = i * 2 + 1, r = l + 1, smallest = i;
            if (l < n && heap.get(l).compareTo(heap.get(smallest)) < 0)
                smallest = l;
            if (r < n && heap.get(r).compareTo(heap.get(smallest)) < 0)
                smallest = r;
            if (smallest == i)
                break;
            swap(i, smallest);
            i = smallest;
        }
    }

    private void removeAt(int pos) {
        int last = heap.size() - 1;
        if (pos < last) {
            swap(pos, last);
            heap.remove(last);
            siftUp(pos);
            siftDown(pos);
        } else {
            heap.remove(last);
        }
    }

    private void swap(int i, int j) {
        E a = heap.get(i);
        E b = heap.get(j);
        idx.put(a, j);
        idx.put(b, i);
    }

    public static void demo() {
        MinHeapBag<Integer> bag = new MinHeapBag<>();
        bag.add(5);
        bag.add(2, 3);
        bag.add(7);
        System.out.println("peekMin=" + bag.peekMin()); // 2
        bag.pollMin(); // 弹出一个 2
        System.out.println("count(2)=" + bag.count(2)); // 2
        bag.remove(2, 2); // 把剩余的 2 全删
        System.out.println("next min=" + bag.peekMin());// 5
    }

}
