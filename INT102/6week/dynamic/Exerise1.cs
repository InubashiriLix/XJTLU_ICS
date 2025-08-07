using System;

namespace ns_exe_1
{
    public class Exercise1
    {
        int a,
            b,
            c;

        public Exercise1(int a, int b, int c)
        {
            this.a = a;
            this.b = b;
            this.c = c;
        }

        public int iter(int time)
        {
            int[] temp = new int[3];
            temp[0] = this.b;
            temp[1] = this.a;
            temp[2] = this.c;

            if (time < 3)
                return temp[time];

            int newValue = 0;
            for (int i = 3; i < time; i++)
            {
                newValue = temp[0] + temp[1] + temp[2];
                temp[0] = temp[1];
                temp[1] = temp[2];
                temp[2] = newValue;
            }

            return newValue;
        }
    }
}
