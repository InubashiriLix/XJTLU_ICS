namespace ns_branch_cut_bag
{

    public class Good
    {
        public int weight, value, dense;
        public Good(int weight, int value, int dense)
        {
            this.weight = weight;
            this.value = value;
            this.dense = dense;
        }

    }
    public class Bag
    {
        private int vol = 50;
        private Good[] goods = { new Good(10, 60, 6), new Good(20, 100, 5), new Good(30, 120, 4) };
        private int upper = 160;
        private List<int> temp_best = new List<int>() { 0, 1 };
        private List<int> temp_path = new List<int>();

        private void brute(int curr, int remain_vol, int curr_value, List<int> temp_path)
        {
            if (remain_vol < 0) { return; }
            if (temp_path.Count > temp_best.Count && curr_value <= upper)
            {
                return;
            }
            if (curr_value > upper)
            {
                temp_best = new List<int>(temp_path);
                upper = curr_value;
            }
            for (int i = curr; i < 3; i++)
            {
                temp_path.Add(i);
                brute(i + 1, remain_vol - goods[i].weight, curr_value + goods[i].value, temp_path);
                temp_path.RemoveAt(temp_path.Count() - 1);
            }
        }

        public Bag()
        {
            // brute force
            brute(0, 50, 0, temp_path);
            Console.WriteLine("best path: {0}, with value: {1}", String.Join(",", temp_best), upper);
        }
    }

}
