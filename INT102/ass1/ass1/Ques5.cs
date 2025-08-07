using System;

namespace ns_ques5
{
    public class Ques5
    {
        public Dictionary<char, List<(char, int)>> adjacencyList = new Dictionary<
            char,
            List<(char, int)>
        >
        {
            ['a'] = new List<(char, int)> { ('b', 4), ('e', 1), ('f', 3) },
            ['b'] = new List<(char, int)> { ('a', 4), ('c', 6), ('f', 10) },
            ['c'] = new List<(char, int)> { ('b', 6), ('d', 7), ('g', 1) },
            ['d'] = new List<(char, int)> { ('c', 7), ('h', 8) },
            ['e'] = new List<(char, int)> { ('a', 1), ('f', 2) },
            ['f'] = new List<(char, int)> { ('e', 2), ('g', 5), ('a', 3), ('b', 10) },
            ['g'] = new List<(char, int)> { ('f', 5), ('h', 7), ('c', 1) },
            ['h'] = new List<(char, int)> { ('d', 8), ('g', 7) },
        };

        public void Dijkstra()
        {
            bool[] visited = new bool[8];
            int[] costs = new int[8];
            Array.Fill(costs, int.MaxValue);
            costs['a' - 'a'] = 0;

            var q = new SortedSet<(int, char)>();
            q.Add((0, 'a'));

            while (q.Count > 0)
            {
                int curr_cost;
                char curr_node;
                (curr_cost, curr_node) = q.Min;
                q.Remove(q.Min);

                int curr_node_idx = curr_node - 'a';
                if (visited[curr_node_idx])
                    continue;
                visited[curr_node_idx] = true;

                if (!adjacencyList.ContainsKey(curr_node))
                    continue;

                foreach (var nei in adjacencyList[curr_node])
                {
                    char next_node = nei.Item1;
                    int next_cost = nei.Item2;
                    int next_node_idx = next_node - 'a';
                    if (!visited[next_node_idx] && costs[next_node_idx] > curr_cost + next_cost)
                    {
                        costs[next_node_idx] = curr_cost + next_cost;
                        q.Add((costs[next_node_idx], next_node));
                    }
                }
                Console.WriteLine(string.Join(" ", costs));
            }
        }

        public void Prim()
        {
            char curr = 'a';
            int[] visited = new int[8];
            Array.Fill(visited, -1);

            int totalCost = 0;

            char next = 'a';
            int next_cost = int.MaxValue;

            while (visited.Count(a => a != -1) > 0)
            {
                visited[curr - 'a'] = totalCost;
                foreach (var next_node in adjacencyList[curr])
                {
                    char temp_next_node = next_node.Item1;
                    int temp_next_node_cost = next_node.Item2;

                    if (visited[temp_next_node - 'a'] == -1 && temp_next_node_cost < next_cost)
                    {
                        next_cost = temp_next_node_cost;
                        next = temp_next_node;
                    }
                }
                curr = next;
                totalCost += next_cost;
            }
            Console.WriteLine(totalCost);
        }

        public Ques5() { }
    }
}
