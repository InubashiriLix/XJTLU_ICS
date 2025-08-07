using System;

namespace ns_gragh
{
    public class Graph
    {
        private int edgeNum;
        public int[][] adjMatrix = new int[8][];
        public Dictionary<int, List<int>> adjList = new Dictionary<int, List<int>>();

        public int[] visitedBFS = new int[8];
        public int[] visitedDFS = new int[8];

        public bool MartixDFS(int idx, int targetIdx, bool[] visited)
        {
            visited[idx] = true;
            if (idx == targetIdx)
                return true;

            bool tempResult = false;

            for (int i = 0; i < 8; i++)
            {
                if (adjMatrix[idx][i] != 0 && visited[i] != true)
                    tempResult = MartixDFS(i, targetIdx, visited.ToArray()) || tempResult;
            }
            return tempResult;
        }

        public bool MatrixBFS(int targetIdx, bool[] visited)
        {
            Queue<int> queue = new Queue<int>();
            queue.Enqueue(0);

            while (queue.Count() > 0)
            {
                int next = queue.Dequeue();
                if (next == targetIdx)
                    return true;
                visited[next] = true;

                for (int i = 0; i < 8; i++)
                {
                    if (adjMatrix[next][i] != 0 && !visited[i])
                    {
                        queue.Enqueue(i);
                    }
                }
            }
            return false;
        }

        public bool AdjListDFS(int curIdx, int targetIdx, bool[] visited)
        {
            if (curIdx == targetIdx)
                return true;
            visited[curIdx] = true;
            bool tempResult = false;
            foreach (int next in adjList[curIdx])
            {
                if (!visited[next])
                {
                    tempResult = AdjListDFS(next, targetIdx, visited.ToArray()) || tempResult;
                }
            }
            return tempResult;
        }

        public bool AdjListBFS(int targetIdx, bool[] visited)
        {
            Queue<int> queue = new Queue<int>();
            queue.Enqueue(0);

            while (queue.Count() > 0)
            {
                int next = queue.Dequeue();
                visited[next] = true;
                if (next == targetIdx)
                    return true;
                foreach (int shit in adjList[next])
                {
                    if (!visited[shit])
                        queue.Enqueue(shit);
                }
            }
            return false;
        }

        public void buildMatrix()
        {
            edgeNum = Convert.ToInt32(Console.ReadLine());
            for (int i = 0; i < 8; i++)
                adjMatrix[i] = new int[8];

            for (int i = 0; i < edgeNum; i++)
            {
                string[] tempInput = Console.ReadLine().Split(' ');
                int a = Convert.ToInt32(tempInput[0]);
                int b = Convert.ToInt32(tempInput[1]);

                adjMatrix[a][b] = 1;
                adjMatrix[b][a] = 1;
            }
        }

        public void buildAdjList()
        {
            edgeNum = Convert.ToInt32(Console.ReadLine());

            for (int i = 0; i < edgeNum; i++)
            {
                string[] tempInput = Console.ReadLine().Split(' ');
                int a = Convert.ToInt32(tempInput[0]);
                int b = Convert.ToInt32(tempInput[1]);

                if (!adjList.ContainsKey(a))
                {
                    adjList.Add(a, new List<int>() { b });
                }
                else
                {
                    adjList[a].Add(b);
                }

                if (!adjList.ContainsKey(b))
                {
                    adjList.Add(b, new List<int>() { a });
                }
                else
                {
                    adjList[b].Add(a);
                }
            }
        }

        /* 10 */
        /* 0 1 */
        /* 1 2 */
        /* 2 3 */
        /* 0 4 */
        /* 1 5 */
        /* 2 6 */
        /* 3 7 */
        /* 4 5 */
        /* 5 6 */
        /* 6 7 */
        public Graph()
        {
            /* buildAdjList(); */
            /**/
            /* for (int i = 0; i < 8; i++) */
            /* { */
            /*     foreach (int j in adjList[i]) */
            /*     { */
            /*         Console.WriteLine("{0} -> {1}", i, j); */
            /*     } */
            /* } */

            /* Console.WriteLine(AdjListDFS(0, 6, new bool[8])); */
            // True
            /* Console.WriteLine(AdjListBFS(6, new bool[8])); */
            // True

            buildMatrix();
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    Console.Write(adjMatrix[i][j] + " ");
                }
                Console.WriteLine();
            }

            /* Console.WriteLine(MatrixBFS(6, new bool[8])); */
            Console.WriteLine(MartixDFS(0, 6, new bool[8]));
        }
    }
}
