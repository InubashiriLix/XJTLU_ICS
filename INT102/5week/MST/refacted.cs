using System;
using System.Linq;

namespace ns_refacted
{
    public static class MST_refacted
    {
        public static void main()
        {
            string[] input = Console.ReadLine().Split(" ");
            int node_num = Convert.ToInt32(input[0]);
            Console.WriteLine("node_num: {0}", node_num);
            int edge_num = Convert.ToInt32(input[1]);
            Console.WriteLine("edge_num: {0}", edge_num);

            // 初始化邻接矩阵
            int[][] map = new int[node_num][];
            for (int i = 0; i < node_num; i++)
            {
                map[i] = new int[node_num];
                for (int j = 0; j < node_num; j++)
                {
                    map[i][j] = int.MaxValue; // 表示无边
                }
            }

            // 读取边
            for (int i = 0; i < edge_num; i++)
            {
                string[] newline = Console.ReadLine().Split(" ");
                int node1 = Convert.ToInt32(newline[0]) - 1; // 索引调整为从 0 开始
                int node2 = Convert.ToInt32(newline[1]) - 1;
                int weight = Convert.ToInt32(newline[2]);
                Console.WriteLine("node1: {0}, node2: {1}, weight: {2}", node1, node2, weight);

                map[node1][node2] = Math.Min(map[node1][node2], weight); // 处理重边
                map[node2][node1] = Math.Min(map[node2][node1], weight);
            }

            bool[] inTree = new bool[node_num];
            for (int i = 0; i < node_num; i++)
            {
                inTree[i] = false;
            }

            int curr_node_idx = 0;
            inTree[curr_node_idx] = true;
            int totalCost = 0;
            for (int i = 0; i < node_num; i++)
            {
                Console.WriteLine("curr_node_idx: {0}", curr_node_idx + 1);
                Console.WriteLine(
                    "inTree: {0}",
                    string.Join(", ", inTree.Select(x => x ? "1" : "0"))
                );
                inTree[curr_node_idx] = true;
                for (int j = 0; j < node_num; j++)
                {
                    // if the next node is not in the current tree
                    // and the next node is smallest one
                    if ((!inTree[j]) && map[curr_node_idx][j] == map[curr_node_idx].Min())
                    {
                        Console.WriteLine(
                            "curr_node_idx: {0}, next_node_idx: {1}, weight: {2}",
                            curr_node_idx + 1,
                            j + 1,
                            map[curr_node_idx][j]
                        );
                        totalCost += map[curr_node_idx].Min();
                        /* map[curr_node_idx][j] = int.MaxValue; */
                        /* map[j][curr_node_idx] = int.MaxValue; */
                        for (int v = 0; v < node_num; v++)
                        {
                            map[curr_node_idx][v] = int.MaxValue;
                            map[v][curr_node_idx] = int.MaxValue;
                        }
                        curr_node_idx = j;
                        break;
                    }
                }
            }
            Console.Write("TotalCost: ");
            Console.Write(totalCost);
        }
    }
}
