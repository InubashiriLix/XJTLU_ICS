using System;
using ns_refacted;

namespace ns_MST
{
    public class MST
    {
        public static void Main()
        {
            /* string[] input = Console.ReadLine().Split(" "); */
            /* int node_num = Convert.ToInt32(input[0]); */
            /* int edge_num = Convert.ToInt32(input[1]); */
            /**/
            /* // 初始化邻接矩阵 */
            /* int[][] map = new int[node_num][]; */
            /* for (int i = 0; i < node_num; i++) */
            /* { */
            /*     map[i] = new int[node_num]; */
            /*     for (int j = 0; j < node_num; j++) */
            /*     { */
            /*         map[i][j] = int.MaxValue; // 表示无边 */
            /*     } */
            /* } */
            /**/
            /* // 读取边 */
            /* for (int i = 0; i < edge_num; i++) */
            /* { */
            /*     string[] newline = Console.ReadLine().Split(" "); */
            /*     int node1 = Convert.ToInt32(newline[0]) - 1; // 索引调整为从 0 开始 */
            /*     int node2 = Convert.ToInt32(newline[1]) - 1; */
            /*     int weight = Convert.ToInt32(newline[2]); */
            /*     Console.WriteLine("node1: {0}, node2: {1}, weight: {2}", node1, node2, weight); */
            /**/
            /*     map[node1][node2] = Math.Min(map[node1][node2], weight); // 处理重边 */
            /*     map[node2][node1] = Math.Min(map[node2][node1], weight); */
            /* } */
            /**/
            /* // Prim 算法初始化 */
            /* bool[] inTree = new bool[node_num]; // 标记节点是否在生成树中 */
            /* int[] minEdge = new int[node_num]; // 存储到生成树的最小边 */
            /* for (int i = 0; i < node_num; i++) */
            /* { */
            /*     minEdge[i] = int.MaxValue; // 初始化为无穷大 */
            /* } */
            /* minEdge[0] = 0; // 从节点 0 开始 */
            /* int totalWeight = 0; */
            /* int edgesUsed = 0; */
            /**/
            /* // 选择最小边的过程 */
            /* for (int i = 0; i < node_num; i++) */
            /* { */
            /*     int u = -1; */
            /*     int minWeight = int.MaxValue; */
            /**/
            /*     // 找出当前未加入生成树的节点中，距离生成树最近的一个 */
            /*     for (int j = 0; j < node_num; j++) */
            /*     { */
            /*         if (!inTree[j] && minEdge[j] < minWeight) */
            /*         { */
            /*             u = j; */
            /*             minWeight = minEdge[j]; */
            /*         } */
            /*     } */
            /**/
            /*     // 如果没有可用的最小边，说明图不连通 */
            /*     if (u == -1) */
            /*     { */
            /*         Console.WriteLine("impossible"); */
            /*         return; */
            /*     } */
            /**/
            /*     // 加入最小生成树 */
            /*     inTree[u] = true; */
            /*     totalWeight += minWeight; */
            /*     edgesUsed++; */
            /**/
            /*     // 更新与树中节点相连的最小边 */
            /*     for (int v = 0; v < node_num; v++) */
            /*     { */
            /*         if (!inTree[v] && map[u][v] != int.MaxValue && map[u][v] < minEdge[v]) */
            /*         { */
            /*             minEdge[v] = map[u][v]; */
            /*         } */
            /*     } */
            /* } */
            /**/
            /* Console.WriteLine("totalWeight"); */
            /* Console.WriteLine(totalWeight); */
            MST_refacted.main();
        }
    }
}
