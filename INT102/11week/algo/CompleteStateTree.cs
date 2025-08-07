namespace ns_complete_state_tree_branch_cut
{
    public class CompleteStateTree
    {
        private int[][] map = new int[4][];

        private int Greedy()
        {
            int[] visited = new int[4];
            int sum = 0;
            for (int i = 0; i < 4; i++)
            {
                var pos = Array.IndexOf(map[i], map[i].Where((val, idx) => visited[idx] == 0).Min());
                visited[pos] = 1;
                sum += map[i][pos];
            }
            return sum;
        }

        private List<int> BackTrack(int lower_bound, int count, List<int> path)
        {
            int sum = 0;
            for (int i = 0; i < Math.Min(4, path.Count); i++)
                sum += map[i][path[i]];
            if (sum >= lower_bound) return null;
            if (count >= 4) return path;

            for (int i = 0; i < 4; i++)
            {
                // exclude the visited points
                if (path.Contains(i)) continue;

                // else not visited
                path.Add(i);
                var res = BackTrack(lower_bound, count + 1, path);
                if (res is not null) { return res; }
                // else this way is not good
                path.RemoveAt(path.Count - 1);
            }
            return null;
        }

        public CompleteStateTree()
        {
            map[0] = new int[] { 9, 2, 7, 8 };
            map[1] = new int[] { 6, 4, 3, 7 };
            map[2] = new int[] { 5, 8, 1, 8 };
            map[3] = new int[] { 7, 6, 9, 4 };

            int lower_bound = Greedy();

            var path = new List<int>();
            BackTrack(lower_bound, 0, path);
            if (path is not null)
            {
                int sum = 0;
                for (int i = 0; i < 4; i++)
                    sum += map[i][path[i]];
                Console.WriteLine("path found: {0}, with min {1}", String.Join(", ", path), sum);
            }
            else
            {
                Console.WriteLine("Not Found");
            }
        }
    }
}
