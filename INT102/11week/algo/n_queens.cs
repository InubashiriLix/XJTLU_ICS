namespace ns_n_queens
{
    public class NQueens
    {

        // private int[][] map = new int[4][];
        private bool dfs(int cnt, List<int[]> path)
        {
            if (path.Count >= 4) return true;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    bool esc = false;
                    foreach (var point in path)
                    {
                        if (i == point[0] || j == point[1] || (Math.Abs(point[0] - i) == Math.Abs(point[1] - j)))
                        { // if aganinst the rule, then escape the j
                            esc = true;
                            continue;
                        }
                    }
                    // if this point is not doable, then go to another point
                    if (esc) continue;
                    // else this point is doable, add this point and seach next point
                    path.Add(new int[] { i, j });
                    if (dfs(cnt + 1, path)) return true;
                    // if failed, then drop the last point and find the another pooint
                    path.RemoveAt(path.Count - 1);
                }
            }
            return false;
        }


        public NQueens()
        {
            var path = new List<int[]>();
            if (dfs(0, path))
            {
                foreach (var point in path)
                    Console.WriteLine("[{0}]", String.Join(", ", point));
            }
            else
                Console.WriteLine("nothing found");
        }
    }
}
