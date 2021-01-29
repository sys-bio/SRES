import pandas as pd
import numpy as np
from sres import SRES
import ctypes as ct
import tellurium as te

# number of generations
NGEN = 100

MODEL = """
model newModel
    r1: A => B; k1*A;
    r2: B => A; k2*B;
    A = 10;
    B = 0.1;
    k1 = 0.1;
    k2 = 0.1;
end
"""

model = te.loada(MODEL)


def getSimulationData(k1: float, k2: float):
    setattr(model, "k1", k1)
    setattr(model, "k2", k2)
    data = model.simulate(0, 10, 11)
    return data


EXP_DATA = getSimulationData(0.1, 0.1)


@ct.CFUNCTYPE(None, ct.POINTER(ct.c_double * 2), ct.POINTER(ct.c_double), ct.POINTER(ct.c_double))
def cost_fun(x, f, g):
    parameter_ids = model.getGlobalParameterIds()
    for parameter_i in range(len(parameter_ids)):
        setattr(model, parameter_ids[parameter_i], x[parameter_i])
    sim = model.simulate(0, 10, 11)
    cost = 0
    for i in range(EXP_DATA.shape[0]):
        for j in range(EXP_DATA.shape[1]):
            cost += (EXP_DATA.iloc[i, j] - sim.iloc[i, j]) ** 2

    # assign cost to the value pointed to by f
    # using this https://stackoverflow.com/a/1556482/3059024
    # how to implement *f = cost?
    # and this: https://stackoverflow.com/a/61543646/3059024
    print("sadfasd")
    f = cost

"""
This callback thing isn't working. Relook atht docs. If no, is there a way I 
can wrap the generation of this function in another function? 
"""


if __name__ == "__main__":
    from sres_capi import *

    sres = SRES()
    esparam = sres._makeESParameter()
    stats = sres._makeESStatistics()
    pop = sres._makeESPopulation()
    # costFun = sres.getCostFunPtr()
    trsf = sres._getTransformFun(2)

    # https://stackoverflow.com/questions/51131433/how-to-pass-lists-into-a-ctypes-function-on-python/51132594
    DoubleArrayLen2 = ct.c_double * 2

    pop_size = 30
    seed = 0
    gamma = 0.85
    alpha = 0.2
    varalphi = 1
    retry = 10
    es = 0

    ub = DoubleArrayLen2(10.0, 10.0)  # double *ub,
    lb = DoubleArrayLen2(0.1, 0.1)  # double *lb,

    print(type(cost_fun))

    sres.ESInitialWithPtrFitnessFcn(
        seed,  # unsigned int seed,
        esparam,  # ESParameter **param,
        trsf,  # ESfcnTrsfm *trsfm,
        cost_fun,  # ESfcnFG* fg,
        es,  # int es,
        0,  # int constraint,
        2,  # int dim,
        ub,
        lb,
        pop_size,  # int miu,
        pop_size,  # int lambda,
        NGEN,  # int gen,
        gamma,  # double gamma,
        alpha,  # double alpha,
        varalphi,  # double varphi,
        retry,  # int retry,
        stats,  # ESPopulation **population
        pop,  # ESStatistics **stats
    )

    # curgen = 0
    # while curgen < NGEN:
    #     sres.ESStep(
    #         sres.derefESPopulation(pop),
    #         sres.derefESParameter(esparam),
    #         sres.derefESStatistics(stats),
    #     )
    #     curgen += 1
    #
    # sres.ESDeInitial(
    #     sres.derefESParameter(esparam),
    #     sres.derefESPopulation(pop),
    #     sres.derefESStatistics(stats)
    # )

    # sres.freeCostFunPtr(costFun)
    sres.freeTransformFun(trsf)
