using System;

namespace ns_ques4
{
    public class Ques4
    {
        public int[,] adjacencyMatrix = new int[6, 6]
        {
            //   a b c d e f  (columns)
            /*a*/{
                0,
                1,
                0,
                0,
                1,
                0,
            },
            /*b*/{ 1, 0, 1, 1, 0, 1 },
            /*c*/{ 0, 1, 0, 1, 0, 1 },
            /*d*/{ 0, 1, 1, 0, 0, 0 },
            /*e*/{ 1, 0, 0, 0, 0, 1 },
            /*f*/{ 0, 1, 1, 0, 1, 0 },
        };

        //----------------------------------------------------------------------
        // 2) Adjacency List
        //    For each vertex, store the list of neighbors.
        //----------------------------------------------------------------------
        // We'll keep the names as 'a','b','c','d','e','f' for clarity.
        public Dictionary<char, List<char>> adjacencyList = new Dictionary<char, List<char>>
        {
            {
                'a',
                new List<char> { 'b', 'e' }
            },
            {
                'b',
                new List<char> { 'a', 'c', 'd', 'f' }
            },
            {
                'c',
                new List<char> { 'b', 'd', 'f' }
            },
            {
                'd',
                new List<char> { 'b', 'c' }
            },
            {
                'e',
                new List<char> { 'a', 'f' }
            },
            {
                'f',
                new List<char> { 'b', 'c', 'e' }
            },
        };

        //----------------------------------------------------------------------
        // 3) Incidence Matrix
        //    We have 6 vertices and 8 edges, so this is a 6x8 matrix.
        //    Each column corresponds to one edge; an undirected edge (u,v)
        //    puts a '1' in row u and row v.
        //----------------------------------------------------------------------
        int[,] incidenceMatrix = new int[6, 8]
        {
            // e1:(0,1), e2:(0,4), e3:(1,5), e4:(1,2), e5:(2,3), e6:(1,3), e7:(2,5), e8:(4,5)
            //   e1 e2 e3 e4 e5 e6 e7 e8   (edges)
            /*a*/{
                1,
                1,
                0,
                0,
                0,
                0,
                0,
                0,
            }, // a=0 is in e1, e2
            /*b*/{ 1, 0, 1, 1, 0, 1, 0, 0 }, // b=1 is in e1, e3, e4, e6
            /*c*/{ 0, 0, 0, 1, 1, 0, 1, 0 }, // c=2 is in e4, e5, e7
            /*d*/{ 0, 0, 0, 0, 1, 1, 0, 0 }, // d=3 is in e5, e6
            /*e*/{ 0, 1, 0, 0, 0, 0, 0, 1 }, // e=4 is in e2, e8
            /*f*/{ 0, 0, 1, 0, 0, 0, 1, 1 }, // f=5 is in e3, e7, e8
        };

        //----------------------------------------------------------------------
        // 4) Incidence List
        //    For each vertex, list which edges (by label) are “incident” on it.
        //    We'll label edges e1..e8 as above.
        //----------------------------------------------------------------------
        public Dictionary<char, List<string>> incidenceList = new Dictionary<char, List<string>>
        {
            {
                'a',
                new List<string> { "e1", "e2" }
            },
            {
                'b',
                new List<string> { "e1", "e3", "e4", "e6" }
            },
            {
                'c',
                new List<string> { "e4", "e5", "e7" }
            },
            {
                'd',
                new List<string> { "e5", "e6" }
            },
            {
                'e',
                new List<string> { "e2", "e8" }
            },
            {
                'f',
                new List<string> { "e3", "e7", "e8" }
            },
        };

        public void BFS_adjMatrix(int start)
        {
            bool[] visited = new bool[6];
            Queue<int> q = new Queue<int>();
            q.Enqueue(start);

            while (q.Count() != 0)
            {
                int next = q.Dequeue();
                visited[next] = true;

                for (int i = 0; i < 6; i++)
                {
                    if (adjacencyMatrix[next, i] == 1 && !visited[i])
                    {
                        q.Enqueue(i);
                    }
                }
            }
            Console.WriteLine("Done");
        }

        public void DFS_adjMatrix(int start, bool[]? visited)
        {
            if (visited == null)
                visited = new bool[6];
            if (!visited[start])
            {
                visited[start] = true;
                for (int i = 0; i < 6; i++)
                {
                    if (adjacencyMatrix[start, i] == 1)
                    {
                        DFS_adjMatrix(i, visited.ToArray());
                    }
                }
            }
            Console.WriteLine("Done");
        }

        public void BFS_adjList(char start)
        {
            bool[] visited = new bool[6];

            Queue<char> q = new Queue<char>();
            q.Enqueue(start);

            while (q.Count() != 0)
            {
                char next = q.Dequeue();
                visited[next - 'a'] = true;
                foreach (char n in adjacencyList[next])
                {
                    if (!visited[n - 'a'])
                    {
                        q.Enqueue(n);
                    }
                }
            }
            Console.WriteLine("Done");
        }

        public void DFS_adjList(char start, bool[]? visited)
        {
            if (visited == null)
                visited = new bool[6];

            if (!visited[start])
            {
                visited[start] = true;
                foreach (char n in adjacencyList[start])
                {
                    if (!visited[n - 'a'])
                    {
                        DFS_adjList(n, visited.ToArray());
                    }
                }
            }
            Console.WriteLine("Done");
        }

        public void BFS_incidencMatrix()
        {
            bool[] visited = new bool[6];

            var q = new Queue<char>();
            q.Enqueue('a');

            while (q.Count > 0)
            {
                var next = q.Dequeue();
                visited[next - 'a'] = true;

                for (int i = 0; i < 8; i++)
                {
                    if (incidenceMatrix[next - 'a', i] == 1)
                    {
                        for (int j = 0; i < 6; j++)
                        {
                            if (incidenceMatrix[j, i] == 1 && !visited[j])
                            {
                                q.Enqueue((char)('a' + j));
                            }
                        }
                    }
                }
            }
            Console.WriteLine("Done");
        }

        public void DFS_incidencMatrix(char start, bool[] visited)
        {
            if (visited[start - 'a'])
                return;

            visited[start - 'a'] = true;

            for (int i = 0; i < 8; i++)
            {
                if (incidenceMatrix[start - 'a', i] == 1)
                {
                    for (int j = 0; j < 6; j++)
                    {
                        if (j != start - 'a' && incidenceMatrix[j, i] == 1)
                        {
                            DFS_incidencMatrix((char)('a' + j), visited.ToArray());
                        }
                    }
                }
            }
            Console.WriteLine("Done");
        }

        public void BFS_incidenceList()
        {
            bool[] visited = new bool[6];
            var q = new Queue<char>();
            q.Enqueue('a');

            while (q.Count > 0)
            {
                var next = q.Dequeue();
                visited[next - 'a'] = true;

                var temp_edgeList = incidenceList[next];

                foreach (string edge_name in temp_edgeList)
                {
                    for (int i = 0; i < 6; i++)
                    {
                        if (i != next - 'a')
                        {
                            foreach (string to_edge_name in incidenceList[(char)('a' + i)])
                            {
                                if (to_edge_name == edge_name && !visited[i])
                                {
                                    q.Enqueue((char)('a' + i));
                                    goto conti;
                                }
                            }
                        }
                    }
                    conti:
                    ;
                }
            }
        }

        public void DFS_incidencList(char start, bool[] visited)
        {
            foreach (string from_edge_name in incidenceList[start])
            {
                for (int i = 0; i < 6; i++)
                {
                    if (i != start - 'a')
                    {
                        foreach (string to_edge_name in incidenceList[(char)('a' + i)])
                        {
                            if (to_edge_name == from_edge_name && !visited[i])
                            {
                                DFS_incidencList((char)('a' + i), visited.ToArray());
                                goto conti;
                            }
                        }
                    }
                }
                conti:
                ;
            }
        }

        public Ques4() { }
    }
}
