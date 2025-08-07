using System;

namespace ns_P2367
{
    class P2367
    {
        public int studentNum;
        public int[] studentScoreDiff;
        public int addTime;

        public P2367()
        {
            string[] line1 = Console.ReadLine().Split(" ");
            studentNum = Convert.ToInt32(line1[0]);
            studentScoreDiff = new int[studentNum + 1];
            studentScoreDiff[0] = 0;
            addTime = Convert.ToInt32(line1[1]);

            int temp = 0;
            string[] line_scores = Console.ReadLine().Split(' ');
            for (int i = 1; i <= studentNum; i++)
            {
                int score = Convert.ToInt32(line_scores[i - 1]);
                studentScoreDiff[i] = score - temp;
                temp = score;
            }

            for (int i = 0; i < addTime; i++)
            {
                string[] line_add = Console.ReadLine().Split(' ');
                int start = Convert.ToInt32(line_add[0]);
                int end = Convert.ToInt32(line_add[1]);
                int add = Convert.ToInt32(line_add[2]);

                studentScoreDiff[start] += add;
                if (end < studentNum)
                {
                    studentScoreDiff[end + 1] -= add;
                }
            }

            int minScore = int.MaxValue;
            int currScore = 0;
            for (int i = 1; i < studentNum + 1; i++)
            {
                currScore += studentScoreDiff[i];
                minScore = Math.Min(minScore, currScore);
            }
            Console.WriteLine(minScore);
        }
    }
}
