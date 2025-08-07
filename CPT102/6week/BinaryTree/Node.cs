namespace ns_node
{
    public class Node
    {
        public Node? left { get; set; }
        public Node? right { get; set; }
        public int? value { get; set; }

        public Node(Node? left, Node? right, int? value)
        {
            this.left = left;
            this.right = right;
            this.value = value;
        }
    }
}
