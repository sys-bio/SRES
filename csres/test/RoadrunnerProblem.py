import os
from typing import List, Tuple, Dict

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
import tellurium as te
from tellurium.roadrunner.extended_roadrunner import ExtendedRoadRunner
import roadrunner
roadrunner.Logger.disableConsoleLogging()
roadrunner.Logger.setLevel(roadrunner.Logger.LOG_CRITICAL)

from csres import SRES

mpl.use("TkAgg")


###################################################################################3
#   highjacking telluriums interface to roadrunner
#

def freeParameters(self):
    """
    Roadrunner models do not have a concept of "free" or "fixed"
    parameters (maybe it should?). Either way, we add a cheeky method
    to the tellurium interface to roadrunner to return the names
    of the parameters we want to fit
    """
    return ["k1", "k2", "k3"]


# assign, and now freeParameters is callable bound to ExtendedRoadrunner types
ExtendedRoadRunner.freeParameters = freeParameters

#####################################################################################3
#   model and data generation
#

# globally scoped model
ANTIMONY_STRING = """
   S1 -> S2; k1*S1;
   S2 -> S3; k2*S2;
   S3 -> S1; k3*S3;
   S1 = 1; S2 = 0; S3 = 0;
   k1 = 0.15; k2 = 0.45; k3 = 15;
"""
r = te.loada(ANTIMONY_STRING)


def get_data(**params) -> Tuple[np.ndarray, np.ndarray, List[str]]:
    r.reset()
    for k, v in params.items():
        setattr(r, k, v)

    try :
        # Simulate "experimental" data
        m = r.simulate(0, 20, 21)
    except Exception:
        return None
    x_data = m["time"]  # only time
    y_data = m[:, 1:]  # remove time
    return x_data, y_data, r.timeCourseSelections[1:]


x_data, y_data, selections = get_data()

# Add random noise to synthetic data
for i in range(0, len(y_data)):
    t = np.random.normal(0, 0.01)
    y_data[i] = y_data[i] + t


##############################################################
#   Configure the optimization
#

@SRES.callback(len(r.freeParameters()))
def cost_fun(parameters):
    """
    Brief
    -----
    Compute difference between experimental dataset and model simulation with candidate parameters.
    This cost function is user defined and used as input to the main SRES algorithm. The input
    to this function is always [parameters, fitness and constraints]. You do not need to worry
    about generating candidate parameters as they are generated by the underlying algorithm. You do
    however have to worry about updating the fitness value, which you do like this:

        fitness.contents.value = calculated_cost

    Where calculated_cost is a float computed by your function. Note, that even though
    we haven't used the constraints argument in this cost function, we still need to pass it in
    as an input parameter.

    Details
    -------
    The underlying SRES C code requires as input a function pointer to a cost function
    that has the following signature:

        typedef void(*ESfcnFG)(double *, double *, double *);

    We can create a cost function in Python to pass to C by using the
    :py:class:`SRES.COST_FUNCTION_CALLBACK` decorator. Since the C end is
    expecting a function with three double pointer types, we must have
    as arguments to our cost function, three arguments.

    When coding the cost function, you need to remember that the types of
    parameter, fitness and constraints are ctypes pointers to double
    arrays in the case of the parameter and constraints argument and
    a pointer to a double in the case of fitness. To do computation
    with these types you need the value that the pointer points to, not
    the pointer. To get these, you use:
        >>> parameters.contents[0]
    In the case of pointer to a double array or
        >>> fitness.contents.value
    in the case of a pointer to a double.

    Args
    ----
    parameters: A list of candidate parameters with the same size as the
                dimensionality of your defined optimization problem.
    fitness:    This is the value that you must compute and assign.

    """
    res = get_data(
        **dict(
            zip(r.freeParameters(), parameters.contents))
    )
    if not res:
        return 10000000
    x, y, sel = res
    # note, you'll need two np.sums when >1 dataset
    cost = np.sum(np.sum((y - y_data) ** 2))

    return cost


def plot(objective_val: List[float], y_sim: np.ndarray, sel: List[str]) -> None:
    colours = ['c', 'm', 'y', 'k']
    fig, axes = plt.subplots(nrows=1, ncols=2)
    print(type(objective_val), type(y_sim), type(sel))
    for i in range(len(sel)):
        name = sel[i]
        axes[0].plot(x_data, y_sim[:, i], label=f"{name}_sim", ls="--", color=colours[i])
        axes[0].plot(x_data, y_data[:, i], label=f"{name}_exp", ls="None", marker='.', color=colours[i])
    axes[0].legend(loc=(-0.7, 0.1))
    axes[0].set_ylabel("AU")
    axes[0].set_xlabel("Time step")
    axes[0].set_title("Best fits")

    axes[1].plot(range(len(objective_val)), np.log10(objective_val), ls='-', marker='o')
    axes[1].set_ylabel("log10 RSS")
    axes[1].set_xlabel("Generation")
    axes[1].set_title("Trace")
    sns.despine(ax=axes[0], top=True, right=True)
    sns.despine(ax=axes[1], top=True, right=True)
    plt.savefig(os.path.join(os.path.dirname(__file__)), bbox_inches='tight', dpi=200)


def do_estimation(ngen: int = 50, popsize: int = 50) -> Tuple[
    List[float], np.ndarray, np.ndarray, List[str], Dict[str, float], float]:
    sres = SRES(
        cost_function=cost_fun,
        popsize=popsize,
        numGenerations=ngen,
        startingValues=[8.3434, 6.342, 3.765] ,
        lb=[0.001] * len(r.freeParameters()),
        ub=[100] * len(r.freeParameters()),
        childrate=7,
    )

    results = sres.fit()
    best = dict(
        zip(r.freeParameters(), results["best_solution"])
    )
    objective_val = results["best_cost"]
    trace = results["trace"]

    x_sim, y_sim, sel = get_data(**best)
    return trace, x_sim, y_sim, sel, best, objective_val


def plot_waterfall(best_values: List[float], stdevs: List[float]):
    best_values = sorted(best_values)
    fig, axes = plt.subplots(nrows=1, ncols=2)
    axes[0].plot(range(len(best_values)), best_values, marker='.', ls="None")
    axes[0].set_ylabel("RSS")
    axes[0].set_xlabel("Ranked best values")
    axes[0].set_title("Waterfall plot (n={})".format(len(best_values)))
    sns.despine(ax=axes[0], top=True, right=True)

    axes[1].plot(range(len(stdevs)), stdevs)
    axes[1].set_ylabel("stdev")
    axes[1].set_xlabel("Successive iterations")
    axes[1].set_title(f"Stdev of best RSS\n(n={len(stdevs)})")
    sns.despine(ax=axes[1], top=True, right=True)

    plt.savefig(os.path.join(
        os.path.dirname(__file__), "waterfall.png"), bbox_inches="tight", dpi=200
    )


def repeated_estimation(ngen: int = 50, n: int = 10):
    # do the first outside the loop to initialize the values
    trace, x_sim, y_sim, sel, best_parameters, best_value = do_estimation()
    best_values: List[float] = [trace[-1]]  # list to store the best values for each PE run
    best_objective_val = trace[-1]  # the best value from best_values
    best_y_sim = y_sim  # the best simulation data from the idx of best value
    std_devs = []  # For collecting running stdev

    for i in range(n - 1):
        # run an estimation
        trace, x_sim, y_sim, sel, best_parameters, best_value = do_estimation(ngen=ngen, popsize=n)
        best_values.append(trace[-1])
        # if the best value for the current run is better than all previous runs
        # we update the best* variables
        if trace[-1] <= min(best_values):
            best_objective_val = trace
            best_y_sim = y_sim
        current_stdev = np.std(best_values)
        std_devs.append(current_stdev)
        print("run:", i,
              ", best objective function value: ", trace[-1],
              ", current stdev of best values so far:", current_stdev,
              ", estimated parameter values: ", best_parameters
              )
    plot_waterfall(best_values, std_devs)
    plot(best_values, best_y_sim, sel)


if __name__ == '__main__':
    # sns.set_context("talk")

    # number of generations for a single parameter estimation
    NGEN = 15

    POPSIZE = 10

    # number of parameter estimations
    N = 150

    DO_SINGLE_ESTIMATION = True
    DO_MULTIPLE_ESTIMATIONS = False

    if DO_SINGLE_ESTIMATION:
        trace, x_sim, y_sim, sel, best_parameters, best_value= do_estimation(ngen=NGEN, popsize=POPSIZE)
        plot(trace, y_sim, sel)

    if DO_MULTIPLE_ESTIMATIONS:
        repeated_estimation(ngen=NGEN, n=N)
