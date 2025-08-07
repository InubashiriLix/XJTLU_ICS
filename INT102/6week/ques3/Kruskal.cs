namespace ns_krus
{
    public class Krus
    {
        private List<(int u, int v, int w)> edges = new List<(int, int, int)>
        {
            (0, 1, 4), // a - b
            (0, 4, 1), // a - e
            (1, 2, 6), // b - c
            (1, 5, 3), // b - f
            (2, 3, 7), // c - d
            (2, 6, 12), // c - g
            (2, 5, 10), // c - f
            (3, 7, 8), // d - h
            (3, 6, 14), // d - g
            (4, 5, 2), // e - f
            (5, 6, 5), // f - g
            (6, 7, 8), // g - h
        };

        private bool[] visitedEdges = new bool[8];
        private bool[] visitedNodes = new bool[8];

        private void _concatEdges()

        private void KrusHandler()
        {
            for (int i = 0; i < edges.Count; i++)
            {
                var edge = edges[i];
            }
        }

        public Krus()
        {
            edges.Sort((a, b) => (a.w).CompareTo(b.w));
            KrusHandler();
        }
    }
}
