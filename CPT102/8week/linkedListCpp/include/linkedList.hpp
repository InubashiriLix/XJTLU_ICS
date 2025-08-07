namespace linkedList {

template <typename T>

struct node {
    node<T> *next;
    node<T> *last;
    T data;
    node(const T &value) : data(value), next(nullptr), last(nullptr){};
};

template <typename T>
class LinkedList {
   public:
    LinkedList(const T &value);
    bool add(const T &value);
    bool add(node<T>);
    bool in(const T value);

   private:
    node<T> *head;
    node<T> *end;
};
}  // namespace linkedList
