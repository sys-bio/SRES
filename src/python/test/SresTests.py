from sres import SRES
import numpy as np
import ctypes as ct

def generateData(mu, sigma):
    return np.random.normal(mu, sigma, 10)


EXP_DATA = generateData(5, 0.1)

sres = SRES(100, [0.01] * 2, [10] * 2)

# decorator for wrapping cost function written in Python
# COST_FUNCTION_CALLBACK = ct.CFUNCTYPE(None, ct.POINTER(ct.c_double * 2), ct.POINTER(ct.c_double),
#                                       ct.POINTER(ct.c_double))


@sres.COST_FUNCTION_CALLBACK
def cost_fun(x, f, g):
    sim = generateData(x.contents[0], x.contents[1])
    cost = 0
    for i in range(10):
        cost += (EXP_DATA[i] - sim[i]) ** 2
    f.contents.value = cost


sres.cost_fun = cost_fun

sres.fit(cost_fun)
