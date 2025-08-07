using System;

namespace ns_test
{
    public class Test
    {
        int[] A;

        public bool BinarySearch(int left, int right, int target)
        {
            if (right < left)
                return false;
            if (right == left)
                return A[left] == target;

            int mid = (left + right) / 2;
            if (A[mid] > target)
                return BinarySearch(left, mid, target);
            if (A[mid] < target)
                return BinarySearch(mid + 1, right, target);
            if (A[mid] == target)
                return true;
            else
                return false;
        }

        public Test()
        {
            A = new int[] { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
            Console.WriteLine(BinarySearch(0, A.Length - 1, 8));
        }
    }
}
