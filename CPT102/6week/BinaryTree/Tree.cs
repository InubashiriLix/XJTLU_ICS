using System;
using System.Collections.Generic;
using System.Linq;
using ns_node;

namespace ns_tree
{
    public class Tree
    {
        public Node[] nodeArray;

        public Node? ConstrPerfBinTree(int curr_idx)
        {
            if (curr_idx >= nodeArray.Length)
                return null;

            nodeArray[curr_idx].left = ConstrPerfBinTree(curr_idx * 2 + 1);
            nodeArray[curr_idx].right = ConstrPerfBinTree(curr_idx * 2 + 2);

            return nodeArray[curr_idx];
        }

        public void ShowNodesVis()
        {
            Console.WriteLine("safsd");
            for (int i = 0; i < nodeArray.Length; i++)
            {
                Console.WriteLine("===============");
                Console.WriteLine("Node: " + nodeArray[i].value);
                if (nodeArray[i].left != null)
                    Console.WriteLine("Left: " + nodeArray[i].left.value);
                if (nodeArray[i].right != null)
                    Console.WriteLine("Right: " + nodeArray[i].right.value);
            }
        }

        private int getExpectedDepth(Node? curr, int dep)
        {
            if (curr == null)
                return dep;
            return getExpectedDepth(curr.left, dep + 1);
        }

        public bool isTreeFull(Node currNode)
        {
            if (currNode.left == null && currNode.right == null)
            {
                return true;
            }
            else if (currNode.left != null && currNode.right != null)
            {
                return isTreeFull(currNode.left) && isTreeFull(currNode.right);
            }
            else
            {
                return false;
            }
        }

        public bool isTreeComplete(Node currNode, int curLevel, int desiredDepth)
        {
            if (curLevel < desiredDepth)
            {
                if (currNode.left == null || currNode.right == null)
                {
                    return false;
                }
                return isTreeComplete(currNode.left, curLevel + 1, desiredDepth)
                    && isTreeComplete(currNode.right, curLevel + 1, desiredDepth);
            }
            else
            {
                if (currNode.left == null && currNode.right == null)
                {
                    return true;
                }
                else if (currNode.left != null && currNode.right == null)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }

        public bool isTreePerfect(Node currNode, int curLevel, int level)
        {
            if (currNode.left == null && currNode.right == null)
                return curLevel == level;
            else if (currNode.left != null && currNode.right != null)
                return isTreePerfect(currNode.left, curLevel + 1, level)
                    && isTreePerfect(currNode.right, curLevel + 1, level);
            else
                return false;
        }

        public Tree()
        {
            int node_num = Convert.ToInt32(Console.ReadLine());

            // create the perfect bianry tree
            nodeArray = new Node[node_num];
            string[] temp_input = Console.ReadLine().Split(' ');
            for (int i = 0; i < node_num; i++)
                nodeArray[i] = new Node(null, null, Convert.ToInt32(temp_input[i]));

            // consturct the node tree
            ConstrPerfBinTree(0);
            ShowNodesVis();
            Console.WriteLine("Expected depth: " + getExpectedDepth(nodeArray[0], 0));
            Console.WriteLine(
                "Is Current Tree Perfect: {0}",
                isTreePerfect(nodeArray[0], 0, getExpectedDepth(nodeArray[0], 0))
            );
            Console.WriteLine(
                "Is Current Tree Complete: {0}",
                isTreeComplete(nodeArray[0], 1, getExpectedDepth(nodeArray[0], 0))
            );
        }
    }
}
