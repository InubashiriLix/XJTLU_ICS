using System;

namespace ns_test
{
    public class Test
    {
        public void MergeSort(int left, int right)
        {
            if (left >= right)
                return;

            int mid = left + (right - left) / 2;
            MergeSort(left, mid);
            MergeSort(mid + 1, right);
            Merge(left, mid, right);
        }

        public void Merge(int left, int mid, int right)
        {
            int[] temp = new int[right - left + 1];
            int i = left,
                j = mid + 1,
                k = 0;
            while (i <= mid && j <= right)
            {
                temp[k++] = A[i] <= A[j] ? A[i++] : A[j++];
            }
            while (i <= mid)
                temp[k++] = A[i++];
            while (j <= right)
                temp[k++] = A[j++];

            for (int t = 0; t < temp.Length; t++)
                A[left + t] = temp[t];
        }

        int[] A = { 38, 27, 43, 3, 9, 82, 10 };

        public Test()
        {
            MergeSort(0, A.Length - 1);
            Console.WriteLine(string.Join(", ", A));
        }
    }
}
