using System;

namespace ns_cfs
{
    class Edge
    {
        public int des;
        public int weight;
        public int next;

        public Edge(int des, int weight, int next)
        {
            this.des = des;
            this.weight = weight;
            this.next = next;
        }
    }

    class CFS
    {
        static Edge[] edges;
        static int[] heads;
        static int cnt = 0;
        static int[] sub_tree;
        static int overall_cost = 0;

        public static void addEdge(int a, int b, int w)
        {
            // add the new edge to the list
            edges[cnt] = new Edge(a, w, heads[b]);
            heads[b] = cnt;
            cnt++;
            edges[cnt] = new Edge(b, w, heads[a]);
            heads[a] = cnt;
            cnt++;
        }

        public static void dfs_cost(int root, int curr)
        {
            sub_tree[root] = 1;

            for (
                int edge_index = heads[curr];
                edge_index != -1;
                edge_index = edges[edge_index].next
            )
            {
                int adjacent = edges[edge_index].next;
                if (adjacent == root)
                    continue;

                dfs_cost(curr, adjacent);

                sub_tree[curr] += sub_tree[adjacent];
                int diff = Math.Abs(sub_tree[adjacent] - (cnt - sub_tree[adjacent]));
                overall_cost += edges[edge_index].weight * diff;
            }
        }

        public CFS()
        {
            int n_edges = Convert.ToInt32(Console.ReadLine());
            edges = new Edge[n_edges * 2];
            heads = new int[n_edges];
            int[] sub_tree = new int[n_edges];
            cnt = 0;

            for (int i = 0; i < n_edges; i++)
            {
                string[] info = Console.ReadLine().Split(' ');
                int a = Convert.ToInt32(info[0]);
                int b = Convert.ToInt32(info[1]);
                int w = Convert.ToInt32(info[2]);
                // save the image
                addEdge(a, b, w);

                // use the dfs to find the overall cost
                dfs_cost(1, -1);
                Console.WriteLine(overall_cost);
            }
        }
    }
}
