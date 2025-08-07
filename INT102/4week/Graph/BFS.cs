using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace ns_bfs
{
    public class BFS
    {
        public BFS()
        {
            string[] temp_header = Console.ReadLine().Split(' ');
            int node_num = Convert.ToInt32(temp_header[0]);
            int edge_num = Convert.ToInt32(temp_header[1]);

            Dictionary<int, List<int[]>> adjacentList = new Dictionary<int, List<int[]>>();

            for (int i = 0; i < edge_num; i++)
            {
                string[] temp = Console.ReadLine().Split(' ');
                int from = Convert.ToInt32(temp[0]);
                int to = Convert.ToInt32(temp[1]);

                if (adjacentList.ContainsKey(from))
                {
                    adjacentList[from].Add(new int[] { to });
                }
                else
                {
                    adjacentList[from] = new List<int[]> { new int[] { to } };
                }

                if (adjacentList.ContainsKey(to))
                {
                    adjacentList[to].Add(new int[] { from });
                }
                else
                {
                    adjacentList[to] = new List<int[]> { new int[] { from } };
                }
            }

            int count = 0;

            void bfs()
            {
                int[] visited = new int[node_num];

                Queue<int> queue = new Queue<int>();
                queue.Enqueue(1);

                while (queue.Count > 0)
                {
                    int node = queue.Dequeue();
                    visited[node - 1] = 1;

                    if (visited.Count(a => a > 0) >= node_num)
                    {
                        count++;
                        break;
                    }

                    if (adjacentList.ContainsKey(node))
                    {
                        foreach (int[] ele in adjacentList[node])
                        {
                            if (visited[ele[0] - 1] == 0)
                            {
                                queue.Enqueue(ele[0]);
                                visited[ele[0] - 1] = 1;
                            }
                        }
                    }
                }
            }

            bfs();
            Console.WriteLine(count);
        }
    }
}
