class Exercise1 {
    public Exercise1(int[] a) {
        // reverse in place
        int len = a.length;
        for (int i = 0; i < len / 2; i++) {
            int temp = a[i];
            a[i] = a[len - i];
            a[len - i] = temp;
        }
    }
}
