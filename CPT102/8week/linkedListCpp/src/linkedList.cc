#include "linkedList.hpp"

template <typename T>
linkedList::LinkedList<T>::LinkedList(const T& value) {
    node<T> newNode = node(value);
    head = &newNode;
    end = &newNode;
}
template <typename T>
bool linkedList::LinkedList<T>::add(const T& value) {
    node<T> newNode = node(value);
    node<T>* temp_next = (*head).next;
    while (1) {
        if (temp_next == nullptr) {
            newNode.last = temp_next;
            temp_next->next = &newNode;
            end = newNode;
            return true;
        }
        temp_next = temp_next->next;
    }
    return false;
}

template <typename T>
bool linkedList::LinkedList<T>::add(node<T> newNode) {
    newNode.last = nullptr;
    newNode.next = nullptr;
    node<T>* temp_next = (*head).next;
    while (1) {
        if (temp_next == nullptr) {
            newNode.last = temp_next;
            temp_next->next = &newNode;
            end = newNode;
            return true;
        }
        temp_next = temp_next->next;
    }
    return false;
}

template <typename T>
bool linkedList::LinkedList<T>::in(const T value) {
    node<T>* temp_next = head;
    while (1) {
        if (temp_next == value.data)
            if (temp_next->next) }
}
