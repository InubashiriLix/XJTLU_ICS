namespace ns_fastpower
{
    public class FastPower
    {
        public int cur(int n, int x)
        {
            if (n == 0)
                return 1;
            return x * cur(n - 1, x);
        }

        public double cur(int n, double x)
        {
            if (n == 0)
                return 1.0;
            return (n % 2 == 0) ? cur(n / 2, x) * cur(n / 2, x) : x * cur(n / 2, x) * cur(n / 2, x);
        }

        public FastPower()
        {
            Console.WriteLine(cur(5, 2));
            Console.WriteLine(cur(2, 0.5));
        }
    }
}
