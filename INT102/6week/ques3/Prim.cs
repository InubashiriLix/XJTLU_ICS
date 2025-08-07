using System;

namespace ns_prim
{
    class Prim
    {
        private Dictionary<int, List<(int, int)>> adjList = new Dictionary<int, List<(int, int)>>()
        {
            {
                0,
                new List<(int, int)> { (1, 4), (4, 1) }
            }, // a
            {
                1,
                new List<(int, int)> { (0, 4), (2, 6), (5, 3) }
            }, // b
            {
                2,
                new List<(int, int)> { (1, 6), (3, 7), (6, 12), (5, 10) }
            }, // c
            {
                3,
                new List<(int, int)> { (2, 7), (7, 8), (6, 14) }
            }, // d
            {
                4,
                new List<(int, int)> { (0, 1), (5, 2) }
            }, // e
            {
                5,
                new List<(int, int)> { (4, 2), (1, 3), (6, 5), (2, 10) }
            }, // f
            {
                6,
                new List<(int, int)> { (5, 5), (2, 12), (3, 14), (7, 8) }
            }, // g
            {
                7,
                new List<(int, int)> { (3, 8), (6, 8) }
            }, // h
        };

        private List<Dictionary<int, int[]>> resultList = new List<Dictionary<int, int[]>>();

        private void PrimHandler(int currIdx, int[] visited, int count, int totalCost)
        {
            if (visited.Count(a => a != 0) >= visited.Length)
            {
                resultList.Add(new Dictionary<int, int[]> { { count, visited } });
                return;
            }

            int next = -1;
            int temp_w = int.MaxValue;
            foreach ((int i, int w) in adjList[currIdx])
            {
                if (visited[i] == 0 && temp_w > w)
                {
                    next = i;
                    temp_w = w;
                    visited[i] = count;
                }
            }
            if (next != -1)
                PrimHandler(next, visited.ToArray(), count + 1, totalCost + temp_w);
        }

        public Prim()
        {
            for (int i = 0; i < adjList.Count; i++)
            {
                PrimHandler(i, new int[adjList.Count], 1, 0);
            }

            foreach (var reuslt in resultList)
            {
                Console.WriteLine($"Cost: {reuslt.Keys.First()}");
                Console.WriteLine($"path: {string.Join(" ", reuslt.Values.First())}");
            }
        }
    }
}
