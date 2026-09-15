public class DelimitedARDeque<T> extends ARDeque<T> {
    DelimitedARDeque(T[] initialItems) {
        super(initialItems);
    }

    DelimitedARDeque() {
        super();
    }

    @Override
    public void addFirst(T item) throws IllegalArgumentException {
        // check whether the item has been in the deque
        if (super.contains(item))
            throw new IllegalArgumentException("Item " + item.toString() + " already exists in the deque");
        super.addFirst(item);
    }

    public static void main(String[] args) {
        DelimitedARDeque<String> dd = new DelimitedARDeque<>();
        try {
            dd.addFirst("apple");
            dd.addLast("banana");
            dd.addFirst("cherry");
            dd.addFirst("apple");
        } catch (IllegalArgumentException e) {
            System.out.println("IllegalArgumentException: " + e.getMessage());
        }
        dd.printDeque();
    }
}
