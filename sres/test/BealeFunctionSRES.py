from sres import SRES
import ctypes as ct

def beale(position):
    """
    optimum at (3.0, 0.5) = 0
    :param position:
    :return:
    """
    x, y = position
    return (1.5 - x + x * y) ** 2 + (2.25 - x + x * y ** 2) ** 2 + (2.625 - x + x * y ** 3) ** 2

best_cost = 10000000

@SRES.COST_FUNCTION_CALLBACK
def cost_fun(parameters, fitness, constraints):
    cost = beale([parameters.contents[0], parameters.contents[1]])
    fitness.contents = ct.c_double(cost)


if __name__ == "__main__":
    ngen = 25
    popsize = 200
    seed = 0
    sres = SRES(
        cost_function=cost_fun,
        seed=0,
        ngen=ngen,
        lb=[-5] * 2,
        ub=[5] * 2,
        parent_popsize=popsize,
        child_popsize=7*popsize,
        gamma=0.85,
        alpha=0.2,
        es=0,
        varphi=1,
        retry=10,
        pf=0.475
    )

    sres.fit(True)












