from scipy.optimize import minimize


def beale(position):
    """
    optimum at (3.0, 0.5) = 0
    :param position:
    :return:
    """
    x, y = position
    return (1.5 - x + x * y) ** 2 + (2.25 - x + x * y ** 2) ** 2 + (2.625 - x + x * y ** 3) ** 2


x0 = [1.0, 1.0]
res = minimize(beale, x0, method="Nelder-Mead")
print(res)
