using System;

namespace ns_counting_sort
{
    public class SHIT
    {
        public SHIT()
        {
            const int input_size = 10;
            int[] input = new int[input_size] { 9, 9, 8, 8, 4, 4, 5, 1, 2, 2 };

            // find the min and the max
            int min = input.Min(),
                max = input.Max();

            // create a counting array
            int[] cnt = new int[max - min + 1];

            // counting shit
            foreach (var shit in input)
                cnt[shit - min]++;

            // rearrange the input array
            int[] output = new int[input_size];
            int temp_idx = 0;
            for (int i = 0; i < max - min + 1; i++)
            {
                if (cnt[i] == 0)
                {
                    continue;
                }
                else
                {
                    for (int j = 0; j < cnt[i]; j++)
                    {
                        output[temp_idx] = i + min;
                        temp_idx++;
                    }
                }
            }
            Console.WriteLine(String.Join(" ", output));
        }
    }
}
