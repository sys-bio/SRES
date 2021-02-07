from scipy.optimize import minimize, dual_annealing
import math


def ackley(X):
    """
    global optimum is at (0, 0) = 0
    :param X:
    :return:
    """
    x1 = X[0]
    x2 = X[1]
    part_1 = -0.2 * math.sqrt(0.5 * (x1 * x1 + x2 * x2))
    part_2 = 0.5 * (math.cos(2 * math.pi * x1) + math.cos(2 * math.pi * x2))
    value = math.exp(1) + 20 - 20 * math.exp(part_1) - math.exp(part_2)
    return value

# note, local optimizer failed
res = dual_annealing(ackley, [(-5, 5), (-5, 5)])
print(res)
