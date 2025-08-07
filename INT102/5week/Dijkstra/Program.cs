using System;
using System.Linq;
using ns_refacted_d;

namespace ns_dikstra
{
    class Dijkstra
    {
        public static void Main()
        {
            Dijkstra_shit.Run();
            /* int[,] matrix = */
            /* { */
            /*     { int.MaxValue, 2, int.MaxValue, 6 }, */
            /*     { 2, int.MaxValue, 3, 2 }, */
            /*     { int.MaxValue, 3, int.MaxValue, 2 }, */
            /*     { 6, 2, 2, int.MaxValue }, */
            /* }; */
            /**/
            /* int length = matrix.GetLength(0); */
            /* int[] node_costs = new int[length]; */
            /* bool[] visited = new bool[length]; */
            /**/
            /* // 初始化所有节点的距离为 int.MaxValue，起点（节点 0）距离为 0 */
            /* Array.Fill(node_costs, int.MaxValue); */
            /* node_costs[0] = 0; */
            /**/
            /* // 对于每个节点，选择距离最小且未处理的节点 */
            /* for (int count = 0; count < length - 1; count++) */
            /* { */
            /*     // 从未访问的节点中选择当前距离最小的节点 u */
            /*     int u = Enumerable */
            /*         .Range(0, length) */
            /*         .Where(i => !visited[i]) */
            /*         .OrderBy(i => node_costs[i]) */
            /*         .First(); */
            /**/
            /*     visited[u] = true; */
            /**/
            /*     // 遍历所有邻居 v，如果 u 到 v 存在边且通过 u 可以缩短 v 的距离，则更新 */
            /*     for (int v = 0; v < length; v++) */
            /*     { */
            /*         if ( */
            /*             !visited[v] */
            /*             && matrix[u, v] != int.MaxValue */
            /*             && node_costs[u] != int.MaxValue */
            /*             && node_costs[u] + matrix[u, v] < node_costs[v] */
            /*         ) */
            /*         { */
            /*             node_costs[v] = node_costs[u] + matrix[u, v]; */
            /*         } */
            /*     } */
            /* } */
            /**/
            /* Console.WriteLine("最短路径代价: " + string.Join(", ", node_costs)); */
        }
    }
}
