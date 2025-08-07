using System;
using System.Collections.Generic;

namespace ns_ham
{
    class Ham
    {
        public Ham()
        {
            var parts = Console.ReadLine().Split();
            int n_nodes = int.Parse(parts[0]);
            int n_edges = int.Parse(parts[1]);

            // 构建邻接表
            var edges = new Dictionary<int, List<int>>();
            for (int i = 0; i < n_edges; i++)
            {
                var line = Console.ReadLine().Split();
                int u = int.Parse(line[0]);
                int v = int.Parse(line[1]);
                if (!edges.ContainsKey(u)) edges[u] = new List<int>();
                if (!edges.ContainsKey(v)) edges[v] = new List<int>();
                edges[u].Add(v);
                edges[v].Add(u);
            }

            var path = new List<int>();
            var cycle = FindHamiltonian(1, n_nodes, edges, path, 1);

            if (cycle == null)
                Console.WriteLine("null found");
            else
                Console.WriteLine(string.Join(" → ", cycle));
        }

        /// <summary>
        /// 从 current 开始，path 记录已走的顶点，origin 是起点。
        /// 找到回路就返回包含回到起点那步的列表，否则返回 null。
        /// </summary>
        static List<int> FindHamiltonian(
            int current,
            int nodeNum,
            Dictionary<int, List<int>> edges,
            List<int> path,
            int origin)
        {
            // 1) 进栈
            path.Add(current);

            // 2) 满足长度时，检查能否回到起点
            if (path.Count == nodeNum)
            {
                if (edges[current].Contains(origin))
                {
                    // 拷贝当前 path，并加上回到 origin 的一步
                    var result = new List<int>(path);
                    result.Add(origin);
                    return result;
                }
                // 剪枝：出栈后返回 null
                path.RemoveAt(path.Count - 1);
                return null;
            }

            // 3) 递归遍历所有邻居
            foreach (var next in edges[current])
            {
                if (!path.Contains(next))
                {
                    var found = FindHamiltonian(next, nodeNum, edges, path, origin);
                    if (found != null)
                        return found;  // 一旦找到就一路返回
                }
            }

            // 4) 回溯出栈
            path.RemoveAt(path.Count - 1);
            return null;
        }
    }
}

