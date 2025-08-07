using System;

namespace ns_UFS
{
    public class UFS
    {
        private int size;
        private int SIZE
        {
            get { return size; }
            set
            {
                if (value <= 0)
                {
                    throw new Exception("Size must be greater than 0");
                }
                else
                {
                    size = value;
                }
            }
        }

        int[] parent;

        public UFS(int size)
        {
            Console.WriteLine("Remember to use true index - 1");
            SIZE = size;
            parent = new int[SIZE];
            for (int i = 0; i < SIZE; i++)
            {
                parent[i] = i;
            }
        }

        public int find(int x)
        {
            if (parent[x] != x)
            {
                parent[x] = find(parent[x]);
            }
            return parent[x];
        }

        public void join(int tar, int par)
        {
            int rootA = find(tar);
            int rootB = find(par);
            if (rootA != rootB)
            {
                parent[rootA] = rootB;
            }
        }
    }
}
