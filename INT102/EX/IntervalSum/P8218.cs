using System;

namespace ns_P8218
{
    class P8218
    {
        public int intervalSize;
        public int[] intervalSum;

        public int ques_num;

        public P8218()
        {
            intervalSize = Convert.ToInt32(Console.ReadLine());
            intervalSum = new int[intervalSize + 1];

            string[] eles = Console.ReadLine().Split(' ');
            int temp = 0;
            for (int i = 0; i < intervalSize; i++)
            {
                temp += Convert.ToInt32(eles[i]);
                intervalSum[i + 1] = temp;
            }

            ques_num = Convert.ToInt32(Console.ReadLine());
            for (int i = 0; i < ques_num; i++)
            {
                string[] ques_eles = Console.ReadLine().Split(' ');
                int low = Convert.ToInt32(ques_eles[0]);
                int high = Convert.ToInt32(ques_eles[1]);
                Console.WriteLine((intervalSum[high] - intervalSum[low - 1]));
            }
        }
    }
}
