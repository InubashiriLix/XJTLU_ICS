using System;
using System.Collections;

namespace ns_Kruska
{
    public class Edge
    {
        public int node1;
        public int node2;
        public int weight;

        public Edge(int node1, int node2, int weight)
        {
            this.node1 = node1;
            this.node2 = node2;
            this.weight = weight;
        }
    }

    class Kruska
    {
        public static void concat(ref int[][] input, int ele1, int ele2, int len)
        {
            for (int i = 0; i < len; i++)
            {
                if (input[ele1][i] == 1 && input[ele2][i] == 1)
                {
                    throw new Exception("Cycle detected");
                }
                else if (input[ele1][i] != input[ele2][i])
                {
                    input[ele1][i] = 1;
                    input[ele2][i] = 0;
                }
            }
        }

        public static bool done(ref int[][] input, int len)
        {
            for (int i = 0; i < input.Length; i++)
            {
                int temp_sum = 0;
                for (int j = 0; j < input.Length; j++)
                {
                    temp_sum += input[i][j];
                }
                if (temp_sum == len)
                {
                    return true;
                }
            }
            return false;
        }

        public static void PrintValues(List<Edge> edges) // Define the PrintValues method
        {
            foreach (var edge in edges)
            {
                Console.WriteLine(
                    "Node1: {0}, Node2: {1}, Weight: {2}",
                    edge.node1,
                    edge.node2,
                    edge.weight
                );
            }
        }

        public static void Main()
        {
            string[] input = Console.ReadLine().Split(" ");
            int node_num = Convert.ToInt32(input[0]);
            Console.WriteLine("node_num: {0}", node_num);
            int edge_num = Convert.ToInt32(input[1]);
            Console.WriteLine("edge_num: {0}", edge_num);

            List<Edge> edges = new List<Edge>();
            // 读取边
            for (int i = 0; i < edge_num; i++)
            {
                string[] newline = Console.ReadLine().Split(" ");
                int node1 = Convert.ToInt32(newline[0]) - 1; // 索引调整为从 0 开始
                int node2 = Convert.ToInt32(newline[1]) - 1;
                int weight = Convert.ToInt32(newline[2]);
                Console.WriteLine("node1: {0}, node2: {1}, weight: {2}", node1, node2, weight);
                edges.Add(new Edge(node1, node2, weight));
            }

            // sorting the fucking edeges
            edges.Sort((e1, e2) => e1.weight.CompareTo(e2.weight));
            PrintValues(edges);

            // initializing the node map
            int[][] nodes = new int[node_num][];
            for (int i = 0; i < node_num; i++)
            {
                nodes[i] = new int[node_num];
                for (int j = 0; j < node_num; j++)
                {
                    nodes[i][j] = 0;
                }
                nodes[i][i] = 1;
            }
            /* [1, 0, 0, 0] */
            /* [0, 1, 0, 0] */
            /* [0, 0, 1, 0] */
            /* [0, 0, 0, 1] */

            int total_cost = 0;
            // enummerate the fucking edges
            for (int i = 0; i < edge_num; i++)
            {
                // if all the nodes are added then break;
                if (done(ref nodes, node_num))
                {
                    Console.WriteLine("Total cost: {0}", total_cost);
                    break;
                }

                // else, then concat the nodes
                total_cost += edges[i].weight;
                concat(ref nodes, edges[i].node1, edges[i].node2, node_num);
            }
        }
    }
}
