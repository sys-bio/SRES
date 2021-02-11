import os
from io import StringIO
from typing import List

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import roadrunner
import seaborn as sns
import tellurium as te
from tellurium.roadrunner.extended_roadrunner import ExtendedRoadRunner

from sres import SRES

mpl.use("TkAgg")

roadrunner.Logger.setLevel(roadrunner.Logger.LOG_CRITICAL)
roadrunner.Logger.disableConsoleLogging()


def freeParameters(self):
    return ["v_0", "ra_0", "kf_0", "kr_0", "Kma_0", "Kms_0", "Kmp_0", "wa_0", "ms_0",
            "mp_0", "v_1", "ri_1", "kf_1", "kr_1", "Kmi_1", "Kms_1", "Kmp_1", "wi_1",
            "ms_1", "mp_1", "v_2", "ri1_2", "ri2_2", "ri3_2", "kf_2", "kr_2",
            "Kmi1_2", "Kmi2_2", "Kmi3_2", "Kms_2", "Kmp_2", "wi1_2", "wi2_2", "wi3_2",
            "ms_2", "mp_2", "v_3", "kf_3", "kr_3", "Kms_3", "Kmp_3", "ms_3", "mp_3"]


ExtendedRoadRunner.freeParameters = freeParameters

r = te.loada("""
    function Fi(v, ri, kf, kr, i, s, p, Kmi, Kms, Kmp, wi, ms, mp)
        ((ri+(1-ri)*(1/(1+i/Kmi)))^wi)*(kf*(s/Kms)^ms-kr*(p/Kmp)^mp)/((1+(s/Kms))^ms+(1+(p/Kmp))^mp-1)
    end
    function F0(v, kf, kr, s, p, Kms, Kmp, ms, mp)
        (kf*(s/Kms)^ms-kr*(p/Kmp)^mp)/((1+(s/Kms))^ms+(1+(p/Kmp))^mp-1)
    end
    function Fa(v, ra, kf, kr, a, s, p, Kma, Kms, Kmp, wa, ms, mp)
        ((ra+(1-ra)*((a/Kma)/(1+a/Kma)))^wa)*(kf*(s/Kms)^ms-kr*(p/Kmp)^mp)/((1+(s/Kms))^ms+(1+(p/Kmp))^mp-1)
    end
    function Fiii(v, ri1, ri2, ri3, kf, kr, i1, i2, i3, s, p, Kmi1, Kmi2, Kmi3, Kms, Kmp, wi1, wi2, wi3, ms, mp)
        ((ri1+(1-ri1)*(1/(1+i1/Kmi1)))^wi1) * ((ri2+(1-ri2)*(1/(1+i2/Kmi2)))^wi2) * ((ri3+(1-ri3)*(1/(1+i3/Kmi3)))^wi3) * (kf*(s/Kms)^ms-kr*(p/Kmp)^mp)/((1+(s/Kms))^ms+(1+(p/Kmp))^mp-1)
    end
    
    model modular_EGFR_current_128()
    
    
    // Reactions
    FreeLigand: -> L; Fa(v_0, ra_0, kf_0, kr_0, Lp, E, L, Kma_0, Kms_0, Kmp_0, wa_0, ms_0, mp_0);
    Phosphotyrosine: -> P; Fi(v_1, ri_1, kf_1, kr_1, Mig6, L, P, Kmi_1, Kms_1, Kmp_1, wi_1, ms_1, mp_1);
    Ras: -> R; Fiii(v_2, ri1_2, ri2_2, ri3_2, kf_2, kr_2, Spry2, P, E, P, R, Kmi1_2, Kmi2_2, Kmi3_2, Kms_2, Kmp_2, wi1_2, wi2_2, wi3_2, ms_2, mp_2);
    Erk: -> E; F0(v_3, kf_3, kr_3, R, E, Kms_3, Kmp_3, ms_3, mp_3);
    
    // Species IVs
    Lp = 100;
    E = 0;
    L = 1000;
    Mig6 = 100;
    P = 0;
    Spry2 = 10000;
    R = 0;
    
    // Parameter values
    v_0 = 1;
    ra_0 = 1;
    kf_0 = 1;
    kr_0 = 1;
    Kma_0 = 1;
    Kms_0 = 1;
    Kmp_0 = 1;
    wa_0 = 1;
    ms_0 = 1;
    mp_0 = 1;
    v_1 = 1;
    ri_1 = 1;
    kf_1 = 1;
    kr_1 = 1;
    Kmi_1 = 1;
    Kms_1 = 1;
    Kmp_1 = 1;
    wi_1 = 1;
    ms_1 = 1;
    mp_1 = 1;
    v_2 = 1;
    ri1_2 = 1;
    ri2_2 = 1;
    ri3_2 = 1;
    kf_2 = 1;
    kr_2 = 1;
    Kmi1_2 = 1;
    Kmi2_2 = 1;
    Kmi3_2 = 1;
    Kms_2 = 1;
    Kmp_2 = 1;
    wi1_2 = 1;
    wi2_2 = 1;
    wi3_2 = 1;
    ms_2 = 1;
    mp_2 = 1;
    v_3 = 1;
    kf_3 = 1;
    kr_3 = 1;
    Kms_3 = 1;
    Kmp_3 = 1;
    ms_3 = 1;
    mp_3 = 1;
    
    end
""")

dataValues = np.genfromtxt(StringIO("""time,L,E,P,R
0,1000,0,0,0
100,887.381,809.746,156.585,2990.51
200,777.382,1651.78,143.552,2731.75
300,678.584,2462.54,131.11,2485.62
400,607.807,3018.03,121.584,2297.79
500,567.493,3204.74,115.929,2186.58
600,545.099,3229.01,112.75,2124.17
700,531.456,3202.71,110.814,2086.18
800,522.145,3164.21,109.495,2060.33
900,515.219,3126.44,108.515,2041.13
1000,509.802,3093.29,107.748,2026.11"""), delimiter=",", skip_header=True)
dataValues_df = pd.DataFrame(dataValues, columns=["time", "L", "E", "P", "R"])
dataValues_df.set_index("time", inplace=True)
# CW get rid of time column for convenience
time = dataValues[:, 1]  # copy for later
dataValues = dataValues[:, 1:]


@SRES.callback(len(r.freeParameters()))
def cost_fun(parameters):
    # Reset the model before
    r.reset()

    # set the new parameter values
    print('sheede')
    for i in range(len(parameters.contents)):
        param = r.freeParameters()[i]
        print(parameters.contents[i])
        val = 10 ** parameters.contents[i]
        # print("param: val: ", param, ":", val, ",")
        setattr(r, param, val)

    # print("\n")
    # compute cost.
    try:
        sim = r.simulate(0, 1000, 11, ["L", "E", "P", "R"])
    except RuntimeError:
        return np.inf
    except Warning:
        return np.inf
    cost = np.sum(np.sum((dataValues - sim) ** 2))

    return cost


def simulate_model(withTheseParameters):
    # set the new parameter values
    for i in range(len(withTheseParameters)):
        param = r.freeParameters()[i]
        val = withTheseParameters[i]
        setattr(r, param, val)
    m = r.simulate(0, 1000, 11)
    return pd.DataFrame(m, columns=m.colnames)


@SRES.callback(len(r.freeParameters()))
def cost_fun_mean_squared(parameters):
    """cost function where each dataset is normalized by mean squared of that dataset"""
    # Reset the model before
    r.reset()

    # set the new parameter values
    for i in range(len(parameters.contents)):
        param = r.freeParameters()[i]
        val = parameters.contents[i]
        setattr(r, param, val)

    # print("\n")
    # compute cost.
    try:
        sim = r.simulate(0, 1000, 11, ["L", "E", "P", "R"])
    except RuntimeError:
        return np.inf
    except Warning:
        return np.inf
    rss = (dataValues - sim) ** 2
    mean_of_cols = dataValues.mean(axis=1)
    for j in range(rss.shape[1]):
        rss[:, j] /= (mean_of_cols[j] ** 2)
    cost = np.sum(np.sum(rss))

    return cost


def do_estimation(ngen: int = 50, popsize: int = 50, starting_set=None):
    sres = SRES(
        cost_function=cost_fun_mean_squared,
        popsize=popsize,
        numGenerations=ngen,
        startingValues=starting_set if starting_set is not None else np.random.normal(5, 0.1, len(r.freeParameters())),
        lb=[0.01] * len(r.freeParameters()),
        ub=[10] * len(r.freeParameters()),
        childrate=7,
        logspace=True,
        verbose=True,
        stopAfterStalledGenerations=0
    )

    return sres.fit()


def plot(results) -> None:
    r.reset()
    for k, v in dict(zip(r.freeParameters(), results["bestSolution"])).items():
        setattr(r, k, v)

    data = r.simulate(0, 1000, 11, ["time", "L", "E", "P", "R"])
    data = pd.DataFrame(data, columns=data.colnames)
    data.set_index("time", inplace=True)

    colours = ['c', 'm', 'y', 'k']
    fig, axes = plt.subplots(nrows=1, ncols=2)
    for i, var in enumerate(data.columns):
        axes[0].plot(data.index, data[var], label=f"{var}_sim", ls="--", color=colours[i])
        axes[0].plot(dataValues_df.index, dataValues_df[var], label=f"{var}_exp", ls="None", marker='.',
                     color=colours[i])
    axes[0].legend(loc=(-0.7, 0.1))
    axes[0].set_ylabel("AU")
    axes[0].set_xlabel("Time step")
    axes[0].set_title("Best fits")

    hof = results["hallOfFame"]
    axes[1].plot(range(len(hof)), np.log10(hof), ls='-', marker='o')
    axes[1].set_ylabel("log10 RSS")
    axes[1].set_xlabel("Update Number")
    axes[1].set_title("Hall Of Fame")
    sns.despine(ax=axes[0], top=True, right=True)
    sns.despine(ax=axes[1], top=True, right=True)
    plt.savefig(os.path.join(os.path.dirname(__file__), "LargeModel.png"), bbox_inches='tight', dpi=200)


def repeated_estimation(ngen: int = 50, popsize: int = 50, n: int = 10):
    # do the first outside the loop to initialize the values
    results = do_estimation()
    best_objective_val = results["bestFitness"]  # the best value from best_values

    best_results = results

    for i in range(n - 1):
        # run an estimation
        results = do_estimation(ngen=ngen, popsize=popsize)
        # if the best value for the current run is better than all previous runs
        # we update the best* variables
        if results["bestFitness"] <= best_objective_val:
            best_results = results
        print("run:", i,
              ", best objective function value: ", results["bestFitness"],
              ", estimated parameter values: ", results["bestSolution"]
              )
        plot(best_results)
    plot_waterfall(results["hallOfFame"])
    print(best_results)


def plot_waterfall(best_fitness_values: List[float]):
    best_fitness_values = sorted(best_fitness_values)
    fig = plt.figure()
    plt.plot(range(len(best_fitness_values)), best_fitness_values, marker='.', ls="None")
    plt.ylabel("RSS")
    plt.xlabel("Ranked best fitness values")
    plt.title("Waterfall plot (n={})".format(len(best_fitness_values)))
    sns.despine(fig=fig, top=True, right=True)

    plt.savefig(os.path.join(
        os.path.dirname(__file__), "waterfall.png"), bbox_inches="tight", dpi=200
    )


if __name__ == "__main__":
    import time

    start = time.time()

    NGEN = 200
    POPSIZE = len(r.freeParameters()) * 10
    N = 20

    DO_PARAMETER_ESTIMATION = True
    DO_MULTIPLE_PARAMETER_ESTIMATIONS = False

    if DO_PARAMETER_ESTIMATION:
        starting_set = [7.02227063, 7.99316053, 4.80914422, 2.60842446, 0.03399503,
                        6.39003928, 9.77369077, 5.21292781, 3.94874179, 6.56794961,
                        4.82594616, 5.6729108, 6.00687204, 0.67424649, 5.09559615,
                        8.79193326, 9.46876033, 6.81484818, 3.77127024, 5.0410533,
                        5.0316706, 8.5122072, 7.38434511, 5.13765973, 7.05435226,
                        7.87489228, 3.28569237, 5.76781861, 3.98673416, 1.50935585,
                        8.71267203, 5.7856511, 5.63214039, 6.07861862, 5.39103686,
                        5.13700395, 5.216952, 7.59655661, 1.53969082, 3.09525245,
                        6.82843622, 3.68719038, 3.81346061]
        results = do_estimation(NGEN, POPSIZE)
        plot(results)
        print(results)

    if DO_MULTIPLE_PARAMETER_ESTIMATIONS:
        repeated_estimation(NGEN, POPSIZE, N)

    print("took ", time.time() - start, "seconds")
    # todo count function evals and nb failed
