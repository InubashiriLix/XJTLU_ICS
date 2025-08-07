package Queue;

import Queue.Iterator;

public class IteratorImplement<E> implements Iterator<E> {

    private int currentIdx;
    private int start;
    private int end;

    public IteratorImplement(int start, int end) {
        this.start = start;
        this.end = end;
        this.currentIdx = start;
    }

    @Override
    public boolean hasNext() {
        return (currentIdx >= start && currentIdx < end);
    }

    @Override
    public E next() {
        // need override in the specific class
        return null;
    }
}
