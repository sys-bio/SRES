from csres import SRES
import ctypes as ct


def beale(position):
    """
    optimum at (3.0, 0.5) = 0
    :param position:
    :return:
    """
    x, y = position
    return (1.5 - x + x * y) ** 2 + (2.25 - x + x * y ** 2) ** 2 + (2.625 - x + x * y ** 3) ** 2


@SRES.callback(2)
def cost_fun(parameters):
    return beale([parameters.contents[0], parameters.contents[1]])



if __name__ == "__main__":
    ngen = 25
    popsize = 200
    seed = 0

    sres = SRES(
        cost_function=cost_fun,
        popsize=20, numGenerations=10,
        startingValues=[9.34, 3.24],
        lb=[0.1]*2, ub=[10]*2
    )

    # sres.fit()
