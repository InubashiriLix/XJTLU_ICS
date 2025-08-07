using System;

namespace ns_CLRS
{
    public class CLRS
    {
        public int[] A = new int[]
        {
            13,
            -3,
            -25,
            20,
            -3,
            -16,
            -23,
            18,
            20,
            -7,
            12,
            -5,
            -22,
            15,
            -4,
            7,
        };

        public CLRS()
        {
            var (l, r, sum) = FindMaxSubArray(A, 0, A.Length - 1);
            Console.WriteLine($"Max subarray: [{l}..{r}], Sum = {sum}");
        }

        public (int left, int right, int sum) FindMaxSubArray(int[] A, int left, int right)
        {
            if (left == right)
                return (left, right, A[left]);

            int mid = (left + right) / 2;

            var leftResult = FindMaxSubArray(A, left, mid);
            var rightResult = FindMaxSubArray(A, mid + 1, right);
            var crossResult = FindMaxCrossingSubarray(A, left, mid, right);

            return leftResult.sum >= rightResult.sum && leftResult.sum >= crossResult.sum
                    ? leftResult
                : rightResult.sum >= crossResult.sum ? rightResult
                : crossResult;
        }

        public (int left, int right, int sum) FindMaxCrossingSubarray(
            int[] A,
            int left,
            int mid,
            int right
        )
        {
            int leftSum = int.MinValue,
                sum = 0,
                maxLeft = mid;
            for (int i = mid; i >= left; i--)
            {
                sum += A[i];
                if (sum > leftSum)
                {
                    leftSum = sum;
                    maxLeft = i;
                }
            }

            int rightSum = int.MinValue;
            sum = 0;
            int maxRight = mid + 1;
            for (int j = mid + 1; j <= right; j++)
            {
                sum += A[j];
                if (sum > rightSum)
                {
                    rightSum = sum;
                    maxRight = j;
                }
            }

            return (maxLeft, maxRight, leftSum + rightSum);
        }
    }
}
