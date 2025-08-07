using System;

namespace ns_P1162
{
    public class P1162
    {
        int n;
        int[][] matrix;
        bool[][] visited;

        int[] moving_dir_x = { 0, 1, 0, -1 };
        int[] moving_dir_y = { -1, 0, 1, 0 };

        public void bfs(int row, int col)
        {
            Queue<int[]> queue = new Queue<int[]>();
            queue.Enqueue(new int[] { row, col });
            visited[row][col] = true;

            while (queue.Count > 0)
            {
                int[] cell = queue.Dequeue();
                int cell_x = cell[0];
                int cell_y = cell[1];

                for (int i = 0; i < 4; i++)
                {
                    int x = cell_x + moving_dir_x[i];
                    int y = cell_y + moving_dir_y[i];

                    if (x >= 0 && x < n && y < n && y >= 0 && matrix[x][y] == 0 && !visited[x][y])
                    {
                        visited[x][y] = true;
                        queue.Enqueue(new int[] { x, y });
                    }
                }
            }
        }

        public P1162()
        {
            n = Convert.ToInt32(Console.ReadLine());
            matrix = new int[n][];
            visited = new bool[n][];
            for (int i = 0; i < n; i++)
            {
                visited[i] = new bool[n];
                matrix[i] = new int[n];
            }

            for (int i = 0; i < n; i++)
            {
                string[] line = Console.ReadLine().Split(' ');
                for (int j = 0; j < n; j++)
                {
                    matrix[i][j] = Convert.ToInt32(line[j]);
                }
            }

            for (int i = 0; i < n; i++)
            {
                if (matrix[i][0] == 0 && !visited[i][0])
                    bfs(i, 0);
                if (matrix[i][n - 1] == 0 && !visited[i][n - 1])
                    bfs(i, n - 1);
            }

            for (int i = 0; i < n; i++)
            {
                if (matrix[0][i] == 0 && !visited[0][i])
                    bfs(0, i);
                if (matrix[n - 1][i] == 0 && !visited[n - 1][i])
                    bfs(n - 1, i);
            }

            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    if (matrix[i][j] == 0 && !visited[i][j])
                    {
                        matrix[i][j] = 2;
                    }
                }
            }

            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    Console.WriteLine(matrix[i][j] + " ");
                }
                Console.Write('\n');
            }
        }
    }
}
