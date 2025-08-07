using System;

namespace ns_ques2
{
    public class Ques2
    {
        private static int[] testArray = new int[] { 12, 23, 36, 49, 51, 55, 99, 107 };
        private int l = 0;
        private int r = testArray.Count() - 1;

        public bool find(int target)
        {
            while (l <= r)
            {
                int mid = (l + r) / 2;
                if (testArray[mid] == target)
                {
                    return true;
                }
                else if (target < testArray[mid])
                { // in the left
                    r = mid - 1;
                }
                else if (target > testArray[mid])
                { // in the right
                    l = mid + 1;
                }
            }
            return false;
        }

        public Ques2()
        {
            Console.WriteLine("shit");
            Console.WriteLine($"finding target num: 23: reslt: {find(23)}");
        }
    }
}
