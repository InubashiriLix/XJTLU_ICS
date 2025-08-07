using ns_exe_1;
using ns_fibonacci_dynamic;

Fibonacci fib = new Fibonacci(1, 2);

int time = 4;

// 1 2 3 5 8
/* Console.WriteLine("recur fib result at {0}: {1}", time, fib.FibRecurssion(time)); */
/* Console.WriteLine("iter fib result at {0}: {1}", time, fib.FibIteration(time)); */
/* Console.WriteLine("recur stored fib result at {0}: {1}", time, fib.FibRecurssuionStored(time - 1)); */

Exercise1 exe = new Exercise1(1, 2, 3);

// 1 2 3 6 11 20
Console.WriteLine("iter exer time {0}: {1}", time, exe.iter(time));
