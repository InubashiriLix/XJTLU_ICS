using System;

namespace ns_factorial
{
    class Factorial
    {
        public int recur(int n)
        {
            if (n == 1)
                return n;
            return n * recur(n - 1);
        }

        public Factorial()
        {
            int n = 5;
            Console.WriteLine(recur(n));
        }
    }
}
