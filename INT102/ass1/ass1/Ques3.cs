using System;

namespace ns_ques3
{
    public class Ques3
    {
        public int[] test1 = new int[] { 6, 1, 2, 3, 4, 5 };

        public void SelectionSortAsseding(ref int[] sortArray)
        {
            int len = test1.Count();
            for (int i = 0; i < len - 1; i++)
            {
                int temp_max_idx = i;
                int temp_max_value = test1[i];

                for (int j = i + 1; j < len; j++)
                {
                    if (temp_max_value > test1[j])
                    {
                        temp_max_idx = j;
                        temp_max_value = test1[j];
                    }
                }

                int temp = test1[i];
                test1[i] = test1[temp_max_idx];
                test1[temp_max_idx] = temp;
            }
            Console.WriteLine(string.Join(" ", test1));
        }

        public void SelectionSortDesending(ref int[] sortArray)
        {
            int len = test1.Count();
            for (int i = 0; i < len - 1; i++)
            {
                int temp_max_idx = i;
                int temp_max_value = test1[i];

                for (int j = i + 1; j < len; j++)
                {
                    if (temp_max_value < test1[j])
                    {
                        temp_max_idx = j;
                        temp_max_value = test1[j];
                    }
                }

                int temp = test1[i];
                test1[i] = test1[temp_max_idx];
                test1[temp_max_idx] = temp;
            }
            Console.WriteLine(string.Join(" ", test1));
        }

        public Ques3()
        {
            SelectionSortAsseding(ref test1);
            SelectionSortDesending(ref test1);
        }
    }
}
