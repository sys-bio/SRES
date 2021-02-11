from sres import SRES

LOGSPACE = True

@SRES.callback(numEstimatedParameters=2)
def beale_cost_fun(parameters):
    """optimum at (x=3.0, y=0.5) = 0"""
    x, y = parameters.contents
    # only when in logspace we need to do this:
    if LOGSPACE:
        x = 10**x
        y = 10**y
    return (1.5 - x + x * y) ** 2 + (2.25 - x + x * y ** 2) ** 2 + (2.625 - x + x * y ** 3) ** 2


if __name__ == "__main__":
    ngen = 5
    popsize = 4

    sres = SRES(
        beale_cost_fun,
        popsize=popsize,
        numGenerations=ngen,
        startingValues=[8.324, 7.335],
        lb=[0.1, 0.1],
        ub=[10, 10],
        childrate=7,
        logspace=LOGSPACE,
        verbose=True,
    )

    print(sres.fit())


