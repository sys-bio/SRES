import numpy as np
import roadrunner
import tellurium as te
from tellurium.roadrunner.extended_roadrunner import ExtendedRoadRunner

from csres import SRES

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

dataValues = csv = np.genfromtxt('SynC.csv', delimiter=",", skip_header=True)
# CW get rid of time column for convenience
time = dataValues[:, 1]  # copy for later
dataValues = dataValues[:, 1:]

failed_evals = 0
total_evals = 0


@SRES.callback(len(r.freeParameters()))
def cost_fun(parameters):
    # Reset the model before
    r.reset()

    # set the new parameter values
    for i in range(len(parameters.contents)):
        param = r.freeParameters()[i]
        val = round(parameters.contents[i], 6)
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


@SRES.callback(len(r.freeParameters()))
def cost_fun_mean_squared(parameters):
    # Reset the model before
    r.reset()

    # set the new parameter values
    for i in range(len(parameters.contents)):
        param = r.freeParameters()[i]
        val = round(parameters.contents[i], 6)
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
    rss = (dataValues - sim) ** 2
    mean_of_cols = dataValues.mean(axis=1)
    for j in range(rss.shape[1]):
        rss[:, j] /= mean_of_cols[j]
    cost = np.sum(np.sum(rss))

    return cost


if __name__ == "__main__":
    import time

    start = time.time()

    ngen = 500
    popsize = len(r.freeParameters()) * 30

    # todo is ngen actually doing anything. Simulations that change this
    #   figure seem very similar.

    best_set = [
        9.55115313, 0.17003503, 5.65980706, 0.62275698, 2.61161608, 2.42138129,
        0.96791146, 2.08587429, 0.75860228, 9.92746723, 0.57231008, 1.49119878,
        0.36182244, 1.72782791, 0.41200846, 3.28571469, 0.93447848, 2.68463832,
        3.9629908, 7.90269899, 4.85671395, 3.55415979, 1.24501534, 6.36069714,
        6.85874704, 1.1541854, 1.09199121, 0.95634204, 0.94469065, 0.09810687,
        2.45212416, 2.65307981, 1.11654523, 1.08075884, 4.29410456, 3.24124077,
        0.4182004, 8.88402939, 0.66741725, 1.42451539, 1.30466651, 7.10731594,
        6.93352265]

    sres = SRES(
        cost_function=cost_fun_mean_squared,
        popsize=popsize,
        numGenerations=ngen,
        # startingValues=np.random.normal(5, 0.1, len(r.freeParameters())),
        startingValues=[
            7.37752164, 0.32413823, 5.82988473, 0.43713322, 1.01093097, 2.35206128,
            0.46532095, 2.28628958, 0.07024239, 9.76933795, 0.44694504, 1.81720957,
            0.71393131, 1.0686815, 0.27038349, 3.57630116, 0.69950115, 2.91349584,
            4.09791179, 7.01174224, 5.54378293, 3.71400609, 1.53214977, 4.13982687,
            7.21297099, 0.62117021, 0.50708, 2.33197804, 0.31233614, 0.22260176,
            0.28006133, 1.99641195, 0.5687386, 1.78217515, 4.80706561, 2.5192148,
            0.86878554, 8.27010811, 0.14919611, 0.80257987, 1.84285307, 5.79002193,
            6.45840006],

        lb=[0.01] * len(r.freeParameters()),
        ub=[10] * len(r.freeParameters()),
        childrate=7,
    )

    results = sres.fit()

    for k, v in results.items():
        print(k, v)

    print("took ", time.time() - start, "seconds")
    # todo count function evals and nb failed
