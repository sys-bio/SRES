from csres import SRES


def beale(position):
    """
    optimum at (3.0, 0.5) = 0
    :param position:
    :return:
    """
    x, y = position
    return (1.5 - x + x * y) ** 2 + (2.25 - x + x * y ** 2) ** 2 + (2.625 - x + x * y ** 3) ** 2


def cost_fun(parameters):
    return beale([parameters.contents[0], parameters.contents[1]])


if __name__ == "__main__":
    ngen = 25
    popsize = 4
    seed = 0

    cb = SRES.callback(2)(cost_fun)

    sres = SRES(
        cb, popsize=popsize, numGenerations=ngen,
        startingValues=[8.324, 7.335],
        lb=[0.1, 0.1],
        ub=[10, 10],
        childrate=7
    )

    print(sres.fit())


