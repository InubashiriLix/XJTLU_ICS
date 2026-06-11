public class Stack<T> {
    private T[] elements;
    private int size;
    public static final int DEFAULT_CAPACITY = 16;

    public Stack() {
        this.elements = (T[]) new Object[DEFAULT_CAPACITY];
        this.size = 0;
    }

    public void push(T value) {
        if (size >= elements.length) {
            T[] temp = (T[]) new Object[elements.length * 2];
            System.arraycopy(elements, 0, temp, 0, elements.length);
            elements = temp;
        }
        elements[size++] = value;
    }

    public T peek() {
        return elements[size - 1];
    }

    public T pop() {
        return elements[--size];

    }

    public int getSize() {
        return size;
    }
};
