from sres import SRES
import numpy as np

def generateData(mu, sigma):
    return np.random.normal(mu, sigma, 10)


EXP_DATA = generateData(5, 0.1)


# decorator for wrapping cost function written in Python
# COST_FUNCTION_CALLBACK = ct.CFUNCTYPE(None, ct.POINTER(ct.c_double * 2), ct.POINTER(ct.c_double),
#                                       ct.POINTER(ct.c_double))


@SRES.COST_FUNCTION_CALLBACK
def cost_fun(x, f, g):
    sim = generateData(x.contents[0], x.contents[1])
    cost = 0
    for i in range(10):
        cost += (EXP_DATA[i] - sim[i]) ** 2
    f.contents.value = cost

sres = SRES(cost_function=cost_fun, ngen=100, lb=[0.01] * 2, ub=[10] * 2, seed=1234)

sres.fit()
