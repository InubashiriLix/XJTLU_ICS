using System;
using System.Collections.Generic;
using System.Linq;

namespace ns_sub_sum
{
    public class SubSum
    {

        private int[] nums = { 3, 4, 5, 6 };
        private int desc = 15;

        private bool BackTrack(int curr, int num, int desc, List<int> path)
        {
            int temp_sum = 0;
            foreach (var index in path)
            {
                temp_sum += nums[index];
            }
            if (temp_sum == desc)
            {
                return true;
            }
            else if (temp_sum > desc)
            {
                return false;
            }
            // else. next step
            for (int i = curr; i < num; i++)
            {
                path.Add(i);
                if (BackTrack(i + 1, num, desc, path))
                {
                    return true;
                }
                path.RemoveAt(path.Count() - 1);
            }
            return false;
        }

        public SubSum()
        {
            var path = new List<int>();
            if (BackTrack(0, nums.Length, desc, path))
            {
                Console.WriteLine("sub set indeies: {0}", String.Join(",", path));
            }
            else
            {
                Console.WriteLine("not found");
            }
        }
    }
}
