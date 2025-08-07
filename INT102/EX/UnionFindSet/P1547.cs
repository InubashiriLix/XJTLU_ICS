using ns_UFS;

namespace ns_1547
{
    public class Edge : IComparable<Edge>
    {
        public int from;
        public int to;
        public int cost;

        public Edge(int from, int to, int cost)
        {
            this.from = from;
            this.to = to;
            this.cost = cost;
        }

        public int CompareTo(Edge other)
        {
            if (other == null)
            {
                throw new Exception("Cannot compare to null");
            }
            if (this.cost == other.cost)
            {
                if (this.from == other.from)
                {
                    return this.to.CompareTo(other.to);
                }
                else
                {
                    return this.from.CompareTo(other.from);
                }
            }
            else
            {
                return this.cost.CompareTo(other.cost);
            }
        }
    }

    public class P1547
    {
        public P1547()
        {
            var edges = new SortedSet<Edge>();
            int node_num,
                edge_num;
            string[] temp = Console.ReadLine().Split(" ");
            node_num = Convert.ToInt32(temp[0]);
            edge_num = Convert.ToInt32(temp[1]);
            var nodes = new UFS(node_num);

            for (int i = 0; i < edge_num; i++)
            {
                string[] temp2 = Console.ReadLine().Split(" ");
                int a = Convert.ToInt32(temp2[0]) - 1;
                int b = Convert.ToInt32(temp2[1]) - 1;
                int c = Convert.ToInt32(temp2[2]);
                edges.Add(new Edge(a, b, c));
            }

            int maxCost = 0;
            int count = 0;

            foreach (var edge in edges)
            {
                if (nodes.find(edge.from) != nodes.find(edge.to))
                {
                    Console.WriteLine("sot");
                    nodes.join(edge.from, edge.to);
                    maxCost = Math.Max(maxCost, edge.cost);
                    count++;
                }
                if (count == node_num - 1)
                {
                    break;
                }
            }

            Console.WriteLine(maxCost);
        }
    }
}
