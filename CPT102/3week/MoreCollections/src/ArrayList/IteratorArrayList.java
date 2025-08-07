package ArrayList;

import java.util.Iterator;

public class IteratorArrayList<T> implements Iterator<T> {
    private int currentIdx = -1;
    private boolean canRemove = false;
    private ArrayList<T> list;

    public IteratorArrayList(ArrayList<T> list) {
        this.list = list;
        this.currentIdx = -1;
        this.canRemove = false;
    }

    @Override
    public boolean hasNext() {
        return (this.currentIdx >= 0 && this.currentIdx < list.size());
    }

    @Override
    public T next() {
        if (!hasNext()) {
            throw new IndexOutOfBoundsException("next has out of boundary");
        }
        currentIdx++;
        canRemove = true;
        return list.get(currentIdx);
    }

    @Override
    public void remove() {
        list.remove(list.get(currentIdx));
        canRemove = false;
    }

}
