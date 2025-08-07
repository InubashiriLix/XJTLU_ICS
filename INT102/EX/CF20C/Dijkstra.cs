using System;

namespace ns_cf20c
{
    public class CF20C
    {
        public int edge_num = 0;
        public int node_num = 0;
        public Dictionary<int, List<int[]>> edgeDict = new Dictionary<int, List<int[]>>();

        public CF20C()
        {
            string[] tempLine = Console.ReadLine().Split(' ');
            node_num = Convert.ToInt32(tempLine[0]);
            edge_num = Convert.ToInt32(tempLine[1]);

            for (int i = 0; i < edge_num; i++)
            {
                tempLine = Console.ReadLine().Split(' ');
                int node1 = Convert.ToInt32(tempLine[0]) - 1;
                int node2 = Convert.ToInt32(tempLine[1]) - 1;
                int weight = Convert.ToInt32(tempLine[2]);

                if (edgeDict.ContainsKey(node1))
                {
                    edgeDict[node1].Add(new int[] { node2, weight });
                }
                else
                {
                    edgeDict.Add(node1, new List<int[]> { new int[] { node2, weight } });
                }

                if (edgeDict.ContainsKey(node2))
                {
                    edgeDict[node2].Add(new int[] { node1, weight });
                }
                else
                {
                    edgeDict.Add(node2, new List<int[]> { new int[] { node1, weight } });
                }
            }
            Dij(0);
        }

        public void Dij(int start)
        {
            bool[] visited;
            int[] costs;
            visited = new bool[node_num];
            costs = new int[node_num];
            Array.Fill(costs, int.MaxValue);
            costs[start] = 0;

            // simple BFS
            /* while (visited.Count(a => a == false) != 0) */
            /* int a = 10; */
            /* while (a-- != 0) */
            /* { */
            /*     List<int> nextVisited = new List<int>(); */
            /*     for (int i = 0; i < node_num; i++) */
            /*     { */
            /*         if (visited[i]) */
            /*         { */
            /*             foreach (int[] edge in edgeDict[i]) */
            /*             { */
            /*                 int edge_to = edge[0]; */
            /*                 int edge_cost = edge[1]; */
            /*                 if (!visited[edge_to]) */
            /*                 { */
            /*                     costs[edge_to] = Math.Min(edge_cost + costs[i], costs[edge_to]); */
            /*                     nextVisited.Add(edge_to); */
            /*                 } */
            /*             } */
            /*         } */
            /*     } */
            /*     foreach (int s in nextVisited) */
            /*     { */
            /*         visited[s] = true; */
            /*     } */
            /**/
            /*     Console.WriteLine(string.Join(" ", costs)); */
            /* } */

            // priority queue
            var pq = new SortedSet<(int cost, int node)>();
            pq.Add((0, start));

            while (pq.Count > 0)
            {
                var (currCost, i) = pq.Min;
                pq.Remove(pq.Min);

                if (visited[i])
                    continue;
                visited[i] = true;

                if (!edgeDict.ContainsKey(i))
                    continue;

                foreach (var edge in edgeDict[i])
                {
                    int v = edge[0],
                        weight = edge[1];
                    if (!visited[v] && costs[v] > costs[i] + weight)
                    {
                        costs[v] = costs[i] + weight;
                        pq.Add((costs[i], v));
                    }
                }
            }
            Console.WriteLine(string.Join(" ", costs));
        }
    }
}
