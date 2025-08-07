// package ArraySet;
//
// import java.util.Collections;
// import java.util.Iterator;
// import java.util.Comparator;
// import java.util.SortedSet;
// import java.util.List;
// import java.util.NoSuchElementException;
// import java.util.ArrayList;
//
// import ArrayList.AbsArrayList;
//
// public class SortedArraySet<E> extends AbsArrayList<E> implements
// SortedSet<E> {
// private final List<E> list;
// private final Comparator<? super E> comparator;
//
// public SortedArraySet() {
// this(null);
// }
//
// public SortedArraySet(Comparator<? super E> comparator) {
// this.comparator = comparator;
// this.list = new ArrayList<>();
// }
//
// @SuppressWarnings("unchecked")
// public int compare(E a, E b) {
// if (comparator != null) {
// // TODO: checck
// return comparator.compare(a, b);
// } else {
// return ((Comparable<? super E>) a).compareTo(b);
// }
// }
//
// @Override
// public boolean add(Object o) {
// E e = (E) o;
// // Object.requireNonNull(e, "element can not be null");
// // binary search
// int idx = Collections.binarySearch(list, e, comparator);
// if (idx >= 0) {
// return false;
// }
//
// int insertPos = -idx - 1;
// list.add(insertPos, e);
// return true;
// }
//
// @Override
// public boolean contains(Object o) {
// if (o == null)
// return false;
// E e = (E) o;
// int idx = Collections.binarySearch(list, e, comparator);
// if (idx >= 0) {
// list.remove(idx);
// return true;
// }
// return false;
// }
//
// @Override
// public Iterator<E> iterator() {
// return Collections.unmodifiableList(list).iterator();
// }
//
// @Override
// public int size() {
// return list.size();
// }
//
// @Override
// public Comparator<? super E> comparator() {
// return comparator;
// }
//
// @Override
// public SortedSet<E> subSet(E fromElement, E toElement) {
// throw new UnsupportedOperationException("subSet not supported");
// }
//
// @Override
// public SortedSet<E> headSet(E toElement) {
// throw new UnsupportedOperationException("shit!");
// }
//
// @Override
// public SortedSet<E> tailSet(E fromElement) {
// throw new UnsupportedOperationException("shit");
// }
//
// @Override
// public E first() {
// if (list.isEmpty())
// throw new NoSuchElementException();
// return list.get(0);
// }
//
// @Override
// public E last() {
// if (list.isEmpty())
// throw new NoSuchElementException();
// return list.get(list.size() - 1);
// }
//
// @Override
// public E get(int index) {
// if (this.isEmpty())
// throw new NoSuchElementException("");
// return list.get(index);
// }
//
// @Override
// public void addAll(Collections<? extends E> collection) {
// for (var ele : collection) {
// this.add(ele);
// }
// }
//
// public void removeAll() {
// while (!this.isEmpty())
// remove(this.get(0));
// }
//
// @Override
// public void removeAll(Collections<?> collection) {
// for (var ele : collection) {
// this.remove(ele);
// }
// }
// }
