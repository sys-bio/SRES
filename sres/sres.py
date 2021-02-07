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
                 childrate: int = 7):
        self._cost_function = cost_function
        self._popsize = popsize
        self._numGenerations = numGenerations
        self._childrate = childrate

        self._numEstimatedParameters = ct.c_int32(len(ub))
        BoundaryArray = (ct.c_double * self._numEstimatedParameters.value)
        self.ub = ct.pointer(BoundaryArray(*ub))
        self.lb = ct.pointer(BoundaryArray(*lb))
        self.startingValues = ct.pointer(BoundaryArray(*startingValues))

        # load the function for creating a SRES obj (doesn't actually call it)
        self._newSRES = self._loadNewSRES()

        # call the C constructor
        self._obj = self._newSRES(
            self._cost_function, self._popsize,
            self._numGenerations, self.startingValues,
            self.lb, self.ub, self._numEstimatedParameters, self._childrate
        )

    @staticmethod
    def callback(numEstimatedParameters: int):
        return ct.CFUNCTYPE(ct.c_double, ct.POINTER(ct.c_double * numEstimatedParameters))

    def getLastError(self) -> str:
        """When an error occurs you can get the message using this method"""
        return self._getLastError()

    def getBestValue(self) -> float:
        """retrieve the best fitness value after fitting"""
        return self._getBestValue(self._obj)

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
        self._fit(self._obj)
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
                SRES.callback(self._numEstimatedParameters.value),
                ct.c_int32,
                ct.c_int32,
                ct.POINTER(ct.c_double * self._numEstimatedParameters.value),
                ct.POINTER(ct.c_double * self._numEstimatedParameters.value),
                ct.POINTER(ct.c_double * self._numEstimatedParameters.value),
                ct.c_int32,
                ct.c_int32
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
    _getBestValue = _sres.load_func(
        funcname="SRES_getBestFitnessValue",
        argtypes=[ct.c_int64],
        return_type=ct.c_double
    )

    _fit = _sres.load_func(
        funcname="SRES_fit",
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
