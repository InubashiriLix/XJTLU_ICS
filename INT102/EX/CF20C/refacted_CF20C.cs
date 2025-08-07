using System;

namespace ns_cf20c_refacted
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
            bool[] visited = new bool[node_num];
            int[] costs = new int[node_num];
            Array.Fill(costs, int.MaxValue);
            costs[start] = 0;

            var pq = new SortedSet<(int cost, int node)>();
            pq.Add((0, start));

            while (pq.Count > 0)
            {
                var (curr_cost, curr) = pq.Min;
                pq.Remove(pq.Min);

                if (visited[curr])
                    continue;
                visited[curr] = true;

                if (!edgeDict.ContainsKey(curr))
                    continue;

                foreach (int[] next in edgeDict[curr])
                {
                    int next_node_idx = next[0];
                    int next_cost = next[1];

                    if (!visited[next_node_idx] && costs[next_node_idx] > curr_cost + next_cost)
                    {
                        costs[next_node_idx] = curr_cost + next_cost;
                        pq.Add((costs[next_node_idx], next_node_idx));
                    }
                }
                Console.WriteLine(string.Join(" ", costs));
            }
        }
    }
}
