import ctypes as ct
import os
from sys import platform
from typing import List, Dict, Union

import numpy as np


class _CSRESLoader:

    def __init__(self):
        self._lib = self._load_lib()

    def _get_shared_library_extension(self):
        if platform == "linux":
            return ".so"
        elif platform == "win32":
            return ".dll"
        elif platform == "darwin":
            return ".dylib"

    def _get_shared_library_prefix(self):
        if platform == "linux":
            return "lib"
        elif platform == "win32":
            return ""
        elif platform == "darwin":
            return "lib"

    def _load_lib(self):
        """Load the SRES C API binary"""
        this_directory = os.path.join(os.path.dirname(__file__))

        shared_lib = os.path.join(
            this_directory,
            f"{self._get_shared_library_prefix()}SRESC{self._get_shared_library_extension()}"
        )

        lib = ct.CDLL(shared_lib)
        return lib

    def load_func(self, funcname: str, argtypes: List, return_type) -> ct.CDLL._FuncPtr:
        """Load a single function from SRES shared library"""
        func = self._lib.__getattr__(funcname)
        func.restype = return_type
        func.argtypes = argtypes
        return func


_NUM_LIST_TYPE = List[Union[float, np.ndarray]]


class SRES:
    _sres = _CSRESLoader()

    def __init__(self, cost_function, popsize: int, numGenerations: int,
                 startingValues: _NUM_LIST_TYPE, lb: _NUM_LIST_TYPE, ub: _NUM_LIST_TYPE,
                 childrate: int = 7, stopAfterStalledGenerations: int = 0,
                 logspace: bool = False,
                 verbose: bool = False):
        """
        Minimize :param cost_function: using the stochastic ranking evolutionary strategy. The main algorithm
        is implemented in C/C++ but the cost function must be user supplied from Python.

        Example
        -------

        >>> from sres import SRES
        >>> @SRES.callback(numEstimatedParameters=2)
        >>> def beale_cost_fun(parameters):
        >>>     x, y = parameters.contents
        >>>     # only when in logspace we need to do this:
        >>>     x = 10**x
        >>>     y = 10**y
        >>>     return (1.5 - x + x * y) ** 2 + (2.25 - x + x * y ** 2) ** 2 + (2.625 - x + x * y ** 3) ** 2
        >>>
        >>> ngen = 100
        >>> popsize = 40
        >>> sres = SRES(
        >>>     beale_cost_fun,
        >>>     popsize=popsize,
        >>>     numGenerations=ngen,
        >>>     startingValues=[8.324, 7.335],
        >>>     lb=[0.1, 0.1],
        >>>     ub=[10, 10],
        >>>     childrate=7,
        >>>     logspace=True,
        >>>     verbose=True,
        >>> )
        >>>
        >>> sres.fit()

        :param cost_function:
            callable. User supplied cost function which takes a single variable which will be a ctypes.c_double
            array of length the same as number of estimated parameters and returns a single float representing
            the quality of the parameter set. Lower is better, the objective function is minimized.
        :param popsize:
            The number of individuals in the population. This number is multiplied by the :param childrate:
            parameter to determine the number of children at each generation.
        :param numGenerations:
            How many generations to run the algorithm for. The algorithm stops after reaching this number of
            generations.
        :param startingValues:
            An initial guess for starting values. These will be a single invididual in the starting population.
            The length of this list must equal the number of estimated parameters.
        :param lb:
            The lower boundaries for parameters during optimization.  The length of this list must equal
             the number of estimated parameters.
        :param ub:
            The upper boundaries for parameters during optimization. The length of this list must
             equal the number of estimated parameters.
        :param childrate:
            Default = 7. Determines the number of children each generation by multiplying with the :param popsize: parameter
        :param stopAfterStalledGenerations:
            Default = 0. Algorithm stops after reaching this many stalled generations. Stalled means no improvement. When
            set to 0, this parameter has no effect. When set to 1, the :param stopAfterStalledGenerations: parameter
            will automatically be 20% of the :param numGenerations: parameter. Otherwise this number is interpreted
            at face value.
        :param logspace:
            Default False. Do optimization in logspace. If set to True, then the user must convert parameters back to linear
            space before using them in the cost function (using 10**x, where x is the value of parameters
            passed to cost function by C). This is beneficial when boundaries span from low to high numbers, like
            1e-3 - 1e3 as it prevents sampling being bias towards large numbers.
        :param verbose:
            Print output after each generation.
        """
        self._cost_function = cost_function
        self._popsize = popsize
        self._numGenerations = numGenerations
        self._childrate = childrate
        self._stopAfterStalledGenerations = stopAfterStalledGenerations
        self._logspace = logspace
        self._verbose = verbose

        self._numEstimatedParameters = ct.c_int32(len(ub))
        BoundaryArray = (ct.c_double * self._numEstimatedParameters.value)
        self.ub = ct.pointer(BoundaryArray(*ub))
        self.lb = ct.pointer(BoundaryArray(*lb))
        self.startingValues = ct.pointer(BoundaryArray(*startingValues))

        # load the function for creating a SRES obj (doesn't actually call it)
        self._newSRES = self._loadNewSRES()

        # call the C constructor
        self._obj = self._newSRES(
            self._cost_function,
            self._popsize,
            self._numGenerations,
            self.startingValues,
            self.lb,
            self.ub,
            self._numEstimatedParameters,
            self._childrate,
            self._stopAfterStalledGenerations,
            self._logspace,
            self._verbose
        )

    @staticmethod
    def callback(numEstimatedParameters: int):
        return ct.CFUNCTYPE(ct.c_double, ct.POINTER(ct.c_double * numEstimatedParameters))

    def getLastError(self) -> str:
        """When an error occurs you can get the message using this method"""
        return self._getLastError()

    def getBestValue(self) -> float:
        """retrieve the best fitness value after fitting"""
        return self._getBestFitnessValue(self._obj)

    def getHallOfFame(self) -> np.array:
        """returns a np.array containing the best fitnesses for each generation"""
        size = self._getSizeOfHallOfFame(self._obj)
        getHof = self._sres.load_func(
            funcname="SRES_getHallOfFame",
            argtypes=[ct.c_int64],
            return_type=ct.POINTER(ct.c_double * size)
        )
        return np.array([i for i in getHof(self._obj).contents],
                        dtype=np.float)

    def getSolution(self) -> np.array:
        """returns a np.array containing the best estimated parameters"""
        _getSol = self._sres.load_func(
            funcname="SRES_getSolution",
            argtypes=[ct.c_int64],
            return_type=ct.POINTER(ct.c_double * self._numEstimatedParameters.value)
        )
        return np.array([i for i in _getSol(self._obj).contents], dtype=np.float)


    def fit(self) -> Dict[str, Union[float, np.array]]:
        """perform the optimization"""
        self._fitLHS(self._obj)
        if self._logspace:
            dct = dict(
                bestFitness=10 ** self.getBestValue(),
                hallOfFame=[10 ** i for i in self.getHallOfFame()],
                bestSolution=[10 ** i for i in self.getSolution()]
            )
        else:
            dct = dict(
                bestFitness=self.getBestValue(),
                hallOfFame=self.getHallOfFame(),
                bestSolution=self.getSolution()
            )
        return dct

    def setSeed(self, seed: int):
        """set the random seed to get predictible parameter estimations"""
        self._setSeed(self._obj, ct.c_ulonglong(seed))

    def _loadNewSRES(self):
        """loads the constructor for SRES algorithm"""
        return self._sres.load_func(
            funcname="SRES_newSRES",
            argtypes=[
                SRES.callback(self._numEstimatedParameters.value),  # CostFunction
                ct.c_int32,  # populationSize
                ct.c_int32,  # numGenerations
                ct.POINTER(ct.c_double * self._numEstimatedParameters.value),  # startingValues
                ct.POINTER(ct.c_double * self._numEstimatedParameters.value),  # lowerBound
                ct.POINTER(ct.c_double * self._numEstimatedParameters.value),  # upperBound
                ct.c_int32,  # num estimated parameters
                ct.c_int32,  # childrate
                ct.c_int32,  # stopAfterStalledGenerations
                ct.c_bool,  # logspace
                ct.c_bool,  # verbose
            ],
            return_type=ct.c_int64
        )

    _getSizeOfHallOfFame = _sres.load_func(
        funcname="SRES_getSizeOfHallOfFame",
        argtypes=[ct.c_int64],
        return_type=ct.c_int32
    )

    _getLastError = _sres.load_func(
        funcname="SRES_getLastError",
        argtypes=[],
        return_type=ct.c_char_p  # pointer to char*
    )

    # https://stackoverflow.com/a/13452473/3059024
    _getBestFitnessValue = _sres.load_func(
        funcname="SRES_getBestFitnessValue",
        argtypes=[ct.c_int64],
        return_type=ct.c_double
    )

    _fit = _sres.load_func(
        funcname="SRES_fit",
        argtypes=[ct.c_int64],
        return_type=ct.c_bool
    )
    _fitLHS = _sres.load_func(
        funcname="SRES_fitLHS",
        argtypes=[ct.c_int64],
        return_type=ct.c_bool
    )

    _setSeed = _sres.load_func(
        funcname="SRES_setSeed",
        argtypes=[ct.c_int64, ct.c_ulonglong],
        return_type=None
    )

    _deleteSRES = _sres.load_func(
        funcname="SRES_deleteSRES",
        argtypes=[ct.c_int64],
        return_type=None
    )

    _freeSolution = _sres.load_func(
        funcname="SRES_freeSolution",
        argtypes=[ct.c_double],
        return_type=None
    )

    _freeHallOfFame = _sres.load_func(
        funcname="SRES_freeHallOfFame",
        argtypes=[ct.c_double],
        return_type=None
    )

    # def __del__(self):
    #     self._deleteSRES(self._obj)
