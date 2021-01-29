import pandas as pd
import numpy as np

#todo look at ths: https://stackoverflow.com/questions/33484591/callbacks-with-ctypes-how-to-call-a-python-function-from-c
# and
import tellurium as te

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
    df = pd.DataFrame(data, columns=data.colnames)
    df.set_index("time")
    return df



def cost_fun(exp: pd.DataFrame, sim: pd.DataFrame):
    cost = 0
    for i in range(exp.shape[0]):
        for j in range(exp.shape[1]):
            cost += (exp.iloc[i, j] - sim.iloc[i, j])**2
    return cost


if __name__ == "__main__":
    exp_data = getSimulationData(0.1, 0.1)

    N = 10

    cost = 100000000

    best_params = []

    # Random selection
    for i in range(N):
        # pick new parameters to test
        # this is the bit that the SRES algo will perform
        k1, k2 = np.random.uniform(0.1, 10, 2)

        cost_tmp = cost_fun(exp_data, getSimulationData(k1, k2))
        print("cost_tmp: ", cost_tmp)

        # update
        if (cost_tmp < cost):
            best_params = [k1, k2]
            cost = cost_tmp

    print("Best cost value is: ", cost)
    print("Best parameters: " , best_params)
    print("Simulation data with best params: ")
    best_df = getSimulationData(best_params[0], best_params[1])
    df_dct = dict(sim=best_df, exp=exp_data)
    print(pd.concat(df_dct, axis=1))





















