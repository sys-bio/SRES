from csres import SRES


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

    cb = SRES.callback(2)(cost_fun)

    # import ctypes as ct
    #
    # lib = ct.CDLL("CCSRES")
    #
    # newSRES = lib.newSRES
    # newSRES.argtypes = [
    #     SRES.callback(2),
    #     ct.c_int32,
    #     ct.c_int32,
    #     ct.POINTER(ct.c_double * 2),
    #     ct.POINTER(ct.c_double * 2),
    #     ct.POINTER(ct.c_double * 2),
    # ]
    # newSRES.restype = ct.c_int64
    #
    # Arr = (ct.c_double) * 2
    # sres = newSRES(
    #     cb, 20, 10, Arr(*[9.65, 3.56]), Arr(*[0.1, 0.1]), Arr(*[10.0, 10.0])
    # )

    sres = SRES(
        cost_function=cb,
        popsize=20, numGenerations=10,
        startingValues=[9.34, 3.24],
        lb=[0.1]*2, ub=[10]*2
    )

    print(sres.fit())


