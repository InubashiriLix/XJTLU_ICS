namespace ns_tower_of_hanoi
{
    public class Tower
    {
        private void cur(int n, string source, string aux, string target)
        {
            if (n == 1)
            {
                Console.WriteLine($"Move disk 1 from {source} to {target}");
                return;
            }

            cur(n - 1, source, target, aux);
            Console.WriteLine($"Move disk {n} from {source} to {target}");
            cur(n - 1, aux, source, target);
        }

        public Tower()
        {
            cur(3, "A", "B", "C");
        }
    }
}
