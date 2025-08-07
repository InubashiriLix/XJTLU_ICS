package HashMap;

import java.lang.Object;
import java.util.Objects;

public class HashMap<K, V> {

    private static final int DEFAULT_CAPACITY = 16;
    private static final float DEFAULT_LOAD_FACTOR = 0.75f;
    private static final int MAXIMUM_CAPACITY = 1 << 30;

    private static class Node<K, V> {
        final int hash;
        final K key;
        V value;
        Node<K, V> next;

        Node(int hash, K key, V value, Node<K, V> next) {
            this.hash = hash;
            this.key = key;
            this.value = value;
            this.next = next;
        }
    }

    private Node<K, V>[] table;
    private int size;
    private int threshold;
    private final float loadFactor;

    public HashMap() {
        this(DEFAULT_CAPACITY, DEFAULT_LOAD_FACTOR);
    }

    public HashMap(int capacity) {
        this(capacity, DEFAULT_LOAD_FACTOR);
    }

    @SuppressWarnings("unchecked")
    public HashMap(int capacity, float loadFactor) {
        if (capacity < 1)
            throw new IllegalArgumentException("capacity must > 0");
        if (loadFactor <= 0 || Float.isNaN(loadFactor)) {
            throw new IllegalArgumentException("load factor must > 0");
        }
        int cap = tableSizeFor(capacity);
        this.loadFactor = loadFactor;
        this.threshold = (int) (cap * loadFactor);
        this.table = (Node<K, V>[]) new Node[cap];
    }

    public int size() {
        return size;
    }

    public boolean isEmpty() {
        return size == 0;
    }

    public V get(Object key) {
        Node<K, V> e = findNode(hash(key), key);
        return e == null ? null : e.value;
    }

    public V put(K key, V value) {
        int hash = hash(key);
        int i = index(hash);
        for (Node<K, V> e = table[i]; e != null; e = e.next) {
            if (e.hash == hash && Objects.equals(e.key, key)) {
                V oldValue = e.value;
                e.value = value;
                return oldValue;
            }
        }
        table[i] = new Node<>(hash, key, value, table[i]);
        if (++size > threshold) {
            resize();
        }
        return null;
    }

    public V remove(Object key) {
        int hash = hash(key);
        int i = index(hash);
        Node<K, V> prev = null;
        Node<K, V> cur = table[i];
        while (cur != null) {
            Node<K, V> next = cur.next;
            if (cur.hash == hash && Objects.equals(cur.key, key)) {
                if (prev == null) {
                    table[i] = next; // Remove head
                } else {
                    prev.next = next; // Remove non-head
                }
                size--;
                return cur.value;

            }
            prev = cur;
            cur = next;
        }
        return null;
    }

    public boolean containsKey(Object key) {
        return findNode(hash(key), key) != null;
    }

    public void clear() {
        @SuppressWarnings("unchecked")
        Node<K, V>[] fresh = (Node<K, V>[]) new Node[table.length];
        table = fresh;
        size = 0;
    }

    private static int hash(Object key) {
        int h = (key == null) ? 0 : key.hashCode();
        return h ^ (h >>> 16);
    }

    private int index(int hash) {
        return hash & (table.length - 1);
    }

    private Node<K, V> findNode(int hash, Object key) {
        for (Node<K, V> e = table[index(hash)]; e != null; e = e.next) {
            if (e.hash == hash && Objects.equals(e.key, key)) {
                return e;
            }
        }
        return null;
    }

    private static int tableSizeFor(int cap) {
        int n = cap - 1;
        n |= n >>> 1;
        n |= n >>> 2;
        n |= n >>> 4;
        n |= n >>> 8;
        n |= n >>> 16;
        return (n < 0) ? 1 : (n >= MAXIMUM_CAPACITY) ? MAXIMUM_CAPACITY : n + 1;
    }

    private void resize() {
        int oldCap = table.length;
        int newCap = oldCap << 1;
        if (oldCap >= MAXIMUM_CAPACITY) {
            threshold = Integer.MAX_VALUE;
            return;
        }

        Node<K, V>[] newTab = (Node<K, V>[]) new Node[newCap];

        for (Node<K, V> head : table) {
            while (head != null) {
                Node<K, V> next = head.next;
                int newIdx = head.hash & (newCap - 1);
                head.next = newTab[newIdx];
                newTab[newIdx] = head;
                head = next;
            }
        }
        table = newTab;
        threshold = (int) (newCap * loadFactor);

    }

    public static void demo() {
        HashMap<String, Integer> map = new HashMap<>();

        map.put("Alice", 1);
        map.put("Bob", 2);
        map.put(null, 99); // null key supported

        System.out.println("Alice = " + map.get("Alice")); // 1
        System.out.println("size  = " + map.size()); // 3

        map.put("Alice", 10); // overwrite
        System.out.println("Alice -> " + map.get("Alice")); // 10

        map.remove("Bob");
        System.out.println("contains Bob? " + map.containsKey("Bob")); // false
        System.out.println("size after rm = " + map.size()); // 2
    }
}
