import java.util.Objects;
import java.util.stream.IntStream;

public class DelimitedARDeque<T> extends ARDeque<T> {
    public boolean contains(T element) {
        return IntStream.range(0, size)
                .mapToObj(i -> items[Math.floorMod(front + i, items.length)]) // 拿到元素
                .filter(Objects::nonNull) // 过滤 null 的元素
                .anyMatch(item -> item.equals(element));
    }

    @Override
    public void addLast(T element) {
        if (contains(element)) {
            throw new IllegalArgumentException("Element already exists in the deque: " + element);
        }
        super.addLast(element);
    }

    public static void main(String[] args) {
        DelimitedARDeque<String> dd = new DelimitedARDeque<>();
        dd.addLast("a");
        dd.addLast("b");
        try {
            dd.addLast("a");
            dd.printDeque();
        } catch (IllegalArgumentException e) {
            System.out.println(e.getMessage());
        }
    }
}
