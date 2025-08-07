using System;

namespace ns_assembly_line_scheduling
{
    public class AssemblyLineScheduling
    {
        class Node
        {
            public int? cross { get; set; }
            public int cost;
            public int line_idx;
            public int line_pos;

            public Node(int? cross, int cost, int line_idx, int line_pos)
            {
                this.cross = cross;
                this.cost = cost;
                this.line_idx = line_idx;
                this.line_pos = line_pos;
            }
        }

        int workingLen;
        Node[] line0;
        Node[] line1;
        Node[][] lines;

        int[] enterCost;

        int[] outCost;

        public AssemblyLineScheduling()
        {
            /* 4               // n = 4, 表示每条装配线有 4 个工位 */
            /* 3 2             // e1=3, e2=2, 进入 Line 1 和 Line 2 的入口时间 */
            /* 1 3             // x1=1, x2=3, 离开 Line 1 和 Line 2 的出口时间 */
            /* 4 5 3 6         // a_{1,1}, a_{1,2}, a_{1,3}, a_{1,4} (Line 1 的各工位处理时间) */
            /* 2 10 1 4        // a_{2,1}, a_{2,2}, a_{2,3}, a_{2,4} (Line 2 的各工位处理时间) */
            /* 7 4 5           // t_{1,1}, t_{1,2}, t_{1,3} (从 Line 1 第 j 工位跨到 Line 2 第 j+1 工位的转换时间) */
            /* 9 2 8           // t_{2,1}, t_{2,2}, t_{2,3} (从 Line 2 第 j 工位跨到 Line 1 第 j+1 工位的转换时间) */

            workingLen = Convert.ToInt32(Console.ReadLine());

            line0 = new Node[workingLen];
            line1 = new Node[workingLen];
            lines = new Node[][] { line0, line1 };

            string[] temp_enter_cost_string = Console.ReadLine().Split(' ');
            enterCost = new int[2]
            {
                Convert.ToInt32(temp_enter_cost_string[0]),
                Convert.ToInt32(temp_enter_cost_string[1]),
            };

            string[] temp_out_cost_string = Console.ReadLine().Split(' ');
            outCost = new int[2]
            {
                Convert.ToInt32(temp_out_cost_string[0]),
                Convert.ToInt32(temp_out_cost_string[1]),
            };

            for (int i = 0; i < 2; i++)
            {
                string[] temp_line_string = Console.ReadLine().Split(' ');
                for (int j = 0; j < workingLen; j++)
                {
                    lines[i][j] = new Node(Convert.ToInt32(temp_line_string[j]), 0, i, j);
                }
            }

            for (int i = 0; i < 2; i++)
            {
                string[] temp_cross_string = Console.ReadLine().Split(' ');
                for (int j = 1; j < workingLen; j++)
                {
                    lines[i][j].cross = Convert.ToInt32(temp_cross_string[j - 1]);
                }
            }

            Dynamic();
        }

        public void Dynamic()
        {
            int totalCost = 0;
            int nextLineIdx = (enterCost[0] + line0[0].cost > enterCost[1] + line1[0].cost) ? 1 : 0;
            totalCost += enterCost[nextLineIdx] + lines[nextLineIdx][0].cost;

            for (int i = 0; i < workingLen - 2; i++)
            {
                if (
                    (lines[nextLineIdx][i].cross + lines[(nextLineIdx + 1) / 2][i + 1].cost)
                    < (lines[nextLineIdx][i + 1].cost)
                )
                {
                    totalCost += (int)(
                        lines[nextLineIdx][i].cross + lines[(nextLineIdx + 1) / 2][i + 1].cost
                    );
                    nextLineIdx = (nextLineIdx + 1) / 2;
                }
                else
                {
                    totalCost += (int)(lines[(nextLineIdx)][i + 1].cost);
                }
            }

            if (
                (
                    lines[nextLineIdx][workingLen - 2].cross
                    + lines[(nextLineIdx + 1) / 2][workingLen - 1].cost
                    + outCost[(nextLineIdx + 1) / 2]
                ) < (lines[nextLineIdx][workingLen - 1].cost)
            )
            {
                totalCost += (int)(
                    lines[nextLineIdx][workingLen - 2].cross
                    + lines[(nextLineIdx + 1) / 2][workingLen - 1].cost
                    + outCost[(nextLineIdx + 1) / 2]
                );
                nextLineIdx = (nextLineIdx + 1) / 2;
            }
            else
            {
                totalCost += (int)(
                    lines[(nextLineIdx)][workingLen - 1].cost + outCost[nextLineIdx]
                );
            }
        }
    }
}
