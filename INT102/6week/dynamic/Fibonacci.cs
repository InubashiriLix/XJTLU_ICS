using System;

namespace ns_fibonacci_dynamic
{
    class Fibonacci
    {
        // NOTE: iter > recur_stored > recur
        // cause' the iter only maintain the temp value and not linking and argument passing are used
        //recur_stored save the time wasted on computing the same value in the recur only situation
        private int A { get; set; }
        private int B { get; set; }

        public Fibonacci(int a, int b)
        {
            A = a;
            B = b;
        }

        public int FibRecurssion(int time)
        {
            if (time == 0)
                return 0;
            if (time == 1)
                return A;
            if (time == 2)
                return B;
            return FibRecurssion(time - 1) + FibRecurssion(time - 2);
        }

        public int FibIteration(int time)
        {
            if (time == 0)
                throw new Exception("Invalid input");
            if (time == 1)
                return A;
            if (time == 2)
                return B;
            if (time == 3)
                return A + B;

            int rtn = A + B;
            int last = B;
            for (int i = 4; i <= time; i++)
            {
                int temp = rtn;
                rtn = rtn + last;
                last = temp;
            }
            return rtn;
        }

        private int[] fibStored;

        public int FibRecurssuionStored(int time)
        {
            fibStored = new int[time + 1];
            Array.Fill(fibStored, -1);
            fibStored[0] = A;
            fibStored[1] = B;
            return (_FibRecurStoredHandler(time));
        }

        private int _FibRecurStoredHandler(int time)
        {
            if (fibStored[time] != -1)
                return fibStored[time];

            fibStored[time] = (_FibRecurStoredHandler(time - 1) + _FibRecurStoredHandler(time - 2));
            return fibStored[time];
        }
    }
}
