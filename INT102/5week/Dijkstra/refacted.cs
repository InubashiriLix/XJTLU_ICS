using System;
using System.Linq;

namespace ns_refacted_d
{
    class Dijkstra_shit
    {
        public static void Run()
        {
            int[,] matrix =
            {
                { int.MaxValue, 2, int.MaxValue, 6 },
                { 2, int.MaxValue, 3, 2 },
                { int.MaxValue, 3, int.MaxValue, 2 },
                { 6, 2, 2, int.MaxValue },
            };

            int len = matrix.GetLength(0);
            int[] costs = new int[len];
            bool[] visited = new bool[len];
            Array.Fill(visited, false);
            Array.Fill(costs, int.MaxValue);
            costs[0] = 0;

            // n nodes, find n - 1 times to find all the min path
            for (int i = 0; i < len - 1; i++)
            {
                var next = Enumerable
                    .Range(0, len)
                    .Where(i => !visited[i])
                    .OrderBy(i => costs[i])
                    .First();
                visited[next] = true;
                // enumrate all the neighbors to find the min value of next node
                for (int v = 0; v < len; v++)
                {
                    if (visited[v])
                        continue;
                    if (matrix[next, v] == int.MaxValue)
                        continue;
                    costs[v] = Math.Min(matrix[next, v] + costs[next], costs[v]);
                }
            }
            Console.WriteLine("最短路径代价: " + string.Join(", ", costs));
        }
    }
}
