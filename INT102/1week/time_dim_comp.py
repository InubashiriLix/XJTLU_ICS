import math
import matplotlib.pyplot as plt


def func_example(size: int):
    return size


def fuct_example_2(size: int):
    return math.log(size) * size


def fuckyou(size: int):
    return math.log(size * size)


function_list = [
    func_example,
    fuct_example_2,
    fuckyou,
]


UPPER_BOND = 1000

for func in function_list:
    plt.plot([func(i) for i in range(1, UPPER_BOND)], label=func.__name__)

plt.legend()

plt.show()
