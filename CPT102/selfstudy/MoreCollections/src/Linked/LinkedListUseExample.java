package Linked;

import Linked.LinkedList;

public class LinkedListUseExample<E> {
    private LinkedList<String> linkedList = new LinkedList<String>();

    public LinkedListUseExample() {
        linkedList.add("one");
        linkedList.add("two");
        linkedList.add("three");
        linkedList.add("four");

        System.out.println(linkedList.get(0));
        System.out.println(linkedList.get(1));
        System.out.println(linkedList.get(2));
        System.out.println(linkedList.get(3));

        for (String temp : linkedList) {
            System.out.print(temp + " ");
        }
    }
}
