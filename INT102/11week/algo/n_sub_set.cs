using System;
using System.Collections.Generic;

namespace ns_all_subset

{
    public class SubSets
    {

        private void BackTrack(int pos, int[] nums, List<int> path, ref List<List<int>> res)
        {
            res.Add(new List<int>(path));
            for (int i = pos; i < nums.Length; i++)
            {
                path.Add(nums[i]);
                BackTrack(i + 1, nums, path, ref res);
                path.RemoveAt(path.Count() - 1);
            }
        }

        public SubSets()
        {
            int pos = 0;
            int[] nums = new int[] { 1, 2, 3 };
            List<int> path = new List<int>();
            List<List<int>> res = new List<List<int>>();
            BackTrack(pos, nums, path, ref res);

            foreach (var temp in res)
            {
                Console.WriteLine("[ {0} ]", String.Join(",", temp));
            }
        }
    }
}
