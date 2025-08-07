using System;

namespace ns_ques2
{
    public class SHIT
    {
        public SHIT(int targetShit)
        {
            int[] shit = { 1, 1, 1, 1 };
            if (targetShit <= 3)
                Console.WriteLine("SHIT smaller than 4: 1");
            for (int i = 3; i < targetShit; i++)
            {
                int tempShit = shit.Sum();
                shit[0] = shit[1];
                shit[1] = shit[2];
                shit[2] = shit[3];
                shit[3] = tempShit;
            }
            Console.WriteLine(shit[3]);
        }
    }
}
