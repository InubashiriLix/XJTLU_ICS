#include <functional>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

template <typename T, typename Compare = std::less<T> >
class MinHeap
{
public:
    MinHeap() = default;

    explicit MinHeap(const std::vector<T>& values) : m_array(values)
    {
        buildHeap();
    }

    MinHeap(std::initializer_list<T> values) : m_array(values)
    {
        buildHeap();
    }

    explicit MinHeap(const T* array, int size)
    {
        if (size < 0) {
            throw std::invalid_argument("size cannot be negative");
        }
        if (array == nullptr && size > 0) {
            throw std::invalid_argument("array cannot be null when size is positive");
        }

        if (size > 0) {
            m_array.assign(array, array + size);
        }
        buildHeap();
    }

    bool empty() const
    {
        return m_array.empty();
    }

    int size() const
    {
        return static_cast<int>(m_array.size());
    }

    const T& top() const
    {
        if (empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return m_array[0];
    }

    void insert(const T& value)
    {
        m_array.push_back(value);
        siftUp(size() - 1);
    }

    T removeMin()
    {
        return removeAt(0);
    }

    T removeAt(int index)
    {
        checkIndex(index);

        T removedValue = m_array[index];
        m_array[index] = m_array.back();
        m_array.pop_back();

        if (index < size()) {
            if (index > 0 && less(m_array[index], m_array[parentIndex(index)])) {
                siftUp(index);
            }
            else {
                heapify(index);
            }
        }

        return removedValue;
    }

    bool getLeftChild(T& child, int index) const
    {
        checkIndex(index);
        int target = leftIndex(index);
        if (target >= size()) {
            return false;
        }

        child = m_array[target];
        return true;
    }

    bool getRightChild(T& child, int index) const
    {
        checkIndex(index);
        int target = rightIndex(index);
        if (target >= size()) {
            return false;
        }

        child = m_array[target];
        return true;
    }

    bool getParent(T& parent, int index) const
    {
        checkIndex(index);
        if (index == 0) {
            return false;
        }

        parent = m_array[parentIndex(index)];
        return true;
    }

    void print() const
    {
        for (int i = 0; i < size(); ++i) {
            std::cout << m_array[i] << " ";
        }
        std::cout << std::endl;
    }

private:
    void buildHeap()
    {
        for (int i = size() / 2 - 1; i >= 0; --i) {
            heapify(i);
        }
    }

    void heapify(int index)
    {
        while (true) {
            int smallest = index;
            int left = leftIndex(index);
            int right = rightIndex(index);

            if (left < size() && less(m_array[left], m_array[smallest])) {
                smallest = left;
            }
            if (right < size() && less(m_array[right], m_array[smallest])) {
                smallest = right;
            }
            if (smallest == index) {
                break;
            }

            swap(index, smallest);
            index = smallest;
        }
    }

    void siftUp(int index)
    {
        while (index > 0) {
            int parent = parentIndex(index);
            if (!less(m_array[index], m_array[parent])) {
                break;
            }

            swap(parent, index);
            index = parent;
        }
    }

    bool less(const T& left, const T& right) const
    {
        return m_compare(left, right);
    }

    void swap(int index1, int index2)
    {
        std::swap(m_array[index1], m_array[index2]);
    }

    int leftIndex(int index) const
    {
        return index * 2 + 1;
    }

    int rightIndex(int index) const
    {
        return index * 2 + 2;
    }

    int parentIndex(int index) const
    {
        return (index - 1) / 2;
    }

    void checkIndex(int index) const
    {
        if (index < 0 || index >= size()) {
            throw std::out_of_range("Heap index out of range");
        }
    }

private:
    std::vector<T> m_array;
    Compare        m_compare;
};

int main()
{
    MinHeap<int> heap({5, 4, 3, 2, 1});

    std::cout << "Initial min heap: ";
    heap.print();

    heap.insert(0);
    std::cout << "After insert 0: ";
    heap.print();

    std::cout << "Remove min: " << heap.removeMin() << std::endl;
    std::cout << "After remove min: ";
    heap.print();

    int leftChild;
    int rightChild;
    if (heap.getLeftChild(leftChild, 0)) {
        std::cout << "Left child of root: " << leftChild << std::endl;
    }
    if (heap.getRightChild(rightChild, 0)) {
        std::cout << "Right child of root: " << rightChild << std::endl;
    }

    MinHeap<std::string> stringHeap({"pear", "apple", "orange"});
    std::cout << "String min: " << stringHeap.top() << std::endl;

    return 0;
}
