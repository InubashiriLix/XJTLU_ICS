package ArrayList;

import java.util.Iterator;

public interface Collection<T> {
    boolean isEmpty();

    int size();

    boolean contains(T element);

    boolean add(T element);

    boolean remove(T element);

    Iterator<T> iterator();
}
