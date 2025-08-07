package ArrayList;

import java.util.Iterator;

public interface List<T> extends Iterable<T>, Collection<T> {
    boolean add(T value);

    T get(int index);

    boolean contains(Object o);

    Iterator<T> iterator();
}
