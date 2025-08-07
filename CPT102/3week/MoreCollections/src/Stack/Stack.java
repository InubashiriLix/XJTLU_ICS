package Stack;

import ArrayList.List;

public interface Stack<T> extends List<T> {
    public void push(T value);

    public T pop();

    public T peek();
}
