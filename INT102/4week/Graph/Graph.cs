using System;
using System.Collections;

namespace ns_graph
{
    public class Graph
    {
        public void AdjacnetList()
        {
            string[] temp = Console.ReadLine().Split(' ');
            int num_nodes = Convert.ToInt32(temp[0]);
            int num_edges = Convert.ToInt32(temp[1]);

            Dictionary<int, List<int[]>> AdjacentList = new Dictionary<int, List<int[]>>();

            for (int i = 0; i < num_edges; i++)
            {
                temp = Console.ReadLine().Split(' ');
                int a = Convert.ToInt32(temp[0]);
                int b = Convert.ToInt32(temp[1]);
                int w = Convert.ToInt32(temp[2]);

                if (!AdjacentList.ContainsKey(a))
                {
                    AdjacentList[a] = new List<int[]>();
                }
                AdjacentList[a].Add(new int[] { b, w });
            }
        }

        public void AdjacentMatrix()
        {
            string[] temp = Console.ReadLine().Split(' ');
            int num_nodes = Convert.ToInt32(temp[0]);
            int num_edges = Convert.ToInt32(temp[1]);

            int[][] matrix = new int[num_nodes][];
            Array.Fill(matrix, new int[num_nodes]);

            for (int i = 0; i < num_edges; i++)
            {
                temp = Console.ReadLine().Split(' ');
                int a = Convert.ToInt32(temp[0]) - 1;
                int b = Convert.ToInt32(temp[1]) - 1;
                int w = Convert.ToInt32(temp[2]);

                matrix[a][b] = w;
            }
        }

        public void IncidenceMatrix()
        {
            string[] temp = Console.ReadLine().Split(' ');
            int num_nodes = Convert.ToInt32(temp[0]);
            int num_edges = Convert.ToInt32(temp[1]);

            int[][] matrixIncidence = new int[num_nodes][];
            Array.Fill(matrixIncidence, new int[num_edges]);

            for (int i = 0; i < num_edges; i++)
            {
                temp = Console.ReadLine().Split(' ');
                int a = Convert.ToInt32(temp[0]) - 1;
                int b = Convert.ToInt32(temp[1]) - 1;
                int w = Convert.ToInt32(temp[2]);

                matrixIncidence[a][i] = 1;
                matrixIncidence[b][i] = -1;
            }
        }

        public void IncidenceList()
        {
            string[] temp = Console.ReadLine().Split(' ');
            int num_nodes = Convert.ToInt32(temp[0]);
            int num_edges = Convert.ToInt32(temp[1]);

            int[][] matrixIncidence = new int[num_nodes][];
            Array.Fill(matrixIncidence, new int[num_edges]);

            List<int[]> list = new List<int[]>();

            for (int i = 0; i < num_edges; i++)
            {
                int a = Convert.ToInt32(temp[0]) - 1;
                int b = Convert.ToInt32(temp[1]) - 1;
                int w = Convert.ToInt32(temp[2]);

                list.Add(new int[] { a, b, w });
            }
        }

        public Graph() { }
    }
}
