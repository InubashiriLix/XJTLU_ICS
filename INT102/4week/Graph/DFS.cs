using System;
using System.Collections.Generic;
using System.Linq;

namespace ns_dfs
{
    public class DFS
    {
        public void AdjacnetMatrix()
        {
            string[] temp_header = Console.ReadLine().Split(' ');
            int node_num = Convert.ToInt32(temp_header[0]);
            int edge_num = Convert.ToInt32(temp_header[1]);

            // NOTE: the gragh is undirected
            int[][] adjacentMatrix = new int[node_num][];
            for (int i = 0; i < node_num; i++)
            {
                adjacentMatrix[i] = new int[node_num];
            }

            List<int[]> resultList = new List<int[]>();

            void dfs(int[] visited, int node, int time)
            {
                visited[node] = time;
                if (visited.Count(a => a > 0) >= node_num)
                    resultList.Add(visited);

                if (adjacentMatrix[node].Count(a => a == 1) == 0)
                    return;

                for (int i = 0; i < node_num; i++)
                {
                    if (adjacentMatrix[node][i] == 1 && visited[i] == 0)
                    {
                        dfs(visited.ToArray(), i, time + 1);
                    }
                }
            }

            for (int i = 0; i < edge_num; i++)
            {
                string[] temp = Console.ReadLine().Split(' ');

                int from = Convert.ToInt32(temp[0]) - 1;
                int to = Convert.ToInt32(temp[1]) - 1;
                adjacentMatrix[from][to] = 1;
                adjacentMatrix[to][from] = 1;
            }

            dfs(new int[node_num], 0, 1);
            foreach (int[] ele in resultList)
            {
                Console.WriteLine(string.Join(" ", ele));
            }
        }

        public void AdjacentList()
        {
            string[] temp_header = Console.ReadLine().Split(' ');
            int node_num = Convert.ToInt32(temp_header[0]);
            int edge_num = Convert.ToInt32(temp_header[1]);

            Dictionary<int, List<int>> adjacentList = new Dictionary<int, List<int>>();

            for (int i = 0; i < edge_num; i++)
            {
                string[] temp = Console.ReadLine().Split(' ');
                int from = Convert.ToInt32(temp[0]) - 1;
                int to = Convert.ToInt32(temp[1]) - 1;

                if (adjacentList.ContainsKey(from))
                {
                    adjacentList[from].Add(to);
                }
                else
                {
                    adjacentList[from] = new List<int> { to };
                }

                if (adjacentList.ContainsKey(to))
                {
                    adjacentList[to].Add(from);
                }
                else
                {
                    adjacentList[to] = new List<int> { from };
                }
            }

            List<int[]> result_list = new List<int[]>();

            void dfs(int node, int[] visited, int count)
            {
                visited[node] = count;

                if (visited.Count(a => a == 0) == 0)
                {
                    result_list.Add(visited);
                    return;
                }

                foreach (int next in adjacentList[node])
                {
                    if (visited[next] == 0)
                    {
                        dfs(next, visited.ToArray(), count + 1);
                    }
                }
            }

            int[] visited = new int[node_num];
            dfs(0, visited, 1);

            Console.WriteLine("Done");

            foreach (int[] ele in result_list)
            {
                Console.WriteLine(String.Join(" ", ele));
            }
        }

        public void IncidenceMatrix()
        {
            string[] temp_header = Console.ReadLine().Split(' ');
            int node_num = Convert.ToInt32(temp_header[0]);
            int edge_num = Convert.ToInt32(temp_header[1]);
            int[][] incidenceMatrix = new int[node_num][];
            for (int i = 0; i < node_num; i++)
            {
                incidenceMatrix[i] = new int[edge_num * 2];
            }

            int temp_index = 0;
            for (int i = 0; i < edge_num; i++)
            {
                string[] temp = Console.ReadLine().Split(' ');
                int from = Convert.ToInt32(temp[0]) - 1;
                int to = Convert.ToInt32(temp[1]) - 1;

                incidenceMatrix[temp_index][from] = 1;
                incidenceMatrix[temp_index][to] = -1;
                temp_index++;
                incidenceMatrix[temp_index][to] = -1;
                incidenceMatrix[temp_index][from] = 1;
                temp_index++;
            }

            void dfs(int edge, int[] visited, int count) { }
        }

        public DFS()
        {
            /* AdjacnetMatrix(); */
            /* AdjacentList(); */
        }
    }
}
