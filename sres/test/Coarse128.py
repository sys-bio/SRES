import tellurium as te
from sres import SRES
from tellurium.roadrunner.extended_roadrunner import ExtendedRoadRunner
import numpy as np


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


# print(dataValues)
#
# print(r.simulate(0, 1000, 11, ["L", "E", "P", "R"]))

@SRES.COST_FUNCTION_CALLBACK
def cost_fun(parameters, fitness, constraints):
    # Reset the model before
    r.reset()

    # set the new parameter values
    for i in range(len(parameters.contents)):
        param = r.freeParameters()[i]
        val = parameters.contents[i]
        setattr(r, param, val)

    # compute cost.
    # CW remove the time column
    sim = r.simulate(0, 1000, 11, ["L", "E", "P", "R"])

    # cw we can use np broadcasting to take care of the details
    # Think its faster too
    cost = np.sum(np.sum((dataValues - sim) ** 2))

    # update fitness. This step is crucial and evey cost function
    # must have this line
    fitness.contents.value = cost


sres = SRES(
    cost_function=cost_fun,
    ngen=1000,
    lb=[0.1] * 43,
    ub=[10] * 43,
    parent_popsize=400,
    child_popsize=400,
    gamma=0.85,
    alpha=0.1,
    pf=0.45,
    varphi=0.1,
    seed=0,
)
pf = 1
for i in range(100):
    f = sres.step(0.45, True)
    print (f)

