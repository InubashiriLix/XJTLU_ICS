using System;

namespace ns_fibonacci
{
    class Fibonacci
    {
        public int recur(int n)
        {
            if (n == 1)
                return 1;
            if (n == 0)
                return 0;
            return recur(n - 2) + recur(n - 1);
        }

        public int enumerate(int n)
        {
            int[] temp = { 0, 1 };
            while (n >= 2)
            {
                int temp_1 = temp[1];
                temp[1] = temp[0] + temp[1];
                temp[0] = temp_1;
                n -= 1;
            }
            return temp[1];
        }

        public Fibonacci()
        {
            int n = 10;
            Console.WriteLine(recur(n));
            Console.WriteLine(enumerate(n));
        }
    }
}
