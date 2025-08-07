using System;
using System.Collections.Generic;
using System.Linq;

namespace bag
{
    public class Item
    {
        public int weight, value;
        public double ratio;
        public Item(int w, int v) {
            weight = w; value = v;
            ratio = (double)v / w;
        }
    }

    public class Bag
    {
        private Item[] items;
        private int capacity;
        private int bestValue = 0;
        private List<int> bestPath = new List<int>();

        public Bag(Item[] items, int cap)
        {
            // 按 ratio 降序
            this.items = items.OrderByDescending(x => x.ratio).ToArray();
            capacity = cap;
            Backtrack(0, 0, 0, new List<int>());
            Console.WriteLine($"最优价值 = {bestValue}");
            Console.WriteLine("选中物品下标(排序后)：[" + string.Join(", ", bestPath) + "]");
        }

        // 分支限界的“上界”：fractional knapsack
        private int Bound(int idx, int currW, int currV)
        {
            int bound = currV;
            int remain = capacity - currW;
            for (int i = idx; i < items.Length; i++)
            {
                if (items[i].weight <= remain)
                {
                    remain -= items[i].weight;
                    bound += items[i].value;
                }
                else
                {
                    bound += (int)(items[i].ratio * remain);
                    break;
                }
            }
            return bound;
        }

        // idx: 当前考虑第几个物品
        // currW: 已用重量, currV: 已获价值
        private void Backtrack(int idx, int currW, int currV, List<int> path)
        {
            // 更新最优
            if (currV > bestValue)
            {
                bestValue = currV;
                bestPath = new List<int>(path);
            }
            if (idx >= items.Length) return;

            // 剪枝：上界不如已知最优，直接丢掉
            int ub = Bound(idx, currW, currV);
            if (ub <= bestValue) return;

            // 1) 选 idx
            if (currW + items[idx].weight <= capacity)
            {
                path.Add(idx);
                Backtrack(idx + 1,
                          currW + items[idx].weight,
                          currV + items[idx].value,
                          path);
                path.RemoveAt(path.Count - 1);
            }
            // 2) 不选 idx
            Backtrack(idx + 1, currW, currV, path);
        }

        public static void Main()
        {
            var items = new Item[] {
                new Item(12,24), new Item(7,13), new Item(11,23),
                new Item(8,15),  new Item(9,16), new Item(14,28),
                new Item(6,11),  new Item(10,17),new Item(5,10),
                new Item(3,6),   new Item(13,26),new Item(4,9),
                new Item(15,30), new Item(2,5),  new Item(1,3)
            };
            new Bag(items, 50);
        }
    }
}

