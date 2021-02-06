import ctypes as ct
import os
import sys
from typing import List


class _CSRESLoader:

    def __init__(self):
        self._lib = self._load_lib()

    def _get_shared_library_extension(self):
        if sys.platform == "linux":
            return ".so"
        elif sys.platform == "win32":
            return ".dll"
        elif sys.platform == "darwin":
            return ".dylib"

    def _get_shared_library_prefix(self):
        if sys.platform == "linux":
            return "lib"
        elif sys.platform == "win32":
            return ""
        elif sys.platform == "darwin":
            return "lib"

    def _load_lib(self):
        """Load the SRES C API binary"""
        this_directory = os.path.join(os.path.dirname(__file__))

        shared_lib = os.path.join(
            this_directory,
            f"{self._get_shared_library_prefix()}CCSRES{self._get_shared_library_extension()}"
        )

        lib = ct.CDLL(shared_lib)
        return lib

    def _load_func(self, funcname: str, argtypes: List, return_type) -> ct.CDLL._FuncPtr:
        """Load a single function from SRES shared library"""
        func = self._lib.__getattr__(funcname)
        func.restype = return_type
        func.argtypes = argtypes
        return func


class SRES:
    _sres = _CSRESLoader()

    def __init__(self, cost_function, popsize: int, numGenerations: int,
                 startingValues: List[float], lb: List[float], ub: List[float],
                 childrate: int = 7):
        self._cost_function = cost_function
        self._numEstimatedParameters = ct.c_int32(len(ub))
        self._popsize = ct.c_int32(popsize)
        self._numGenerations = ct.c_int32(numGenerations)
        self._childrate = ct.c_int32(childrate)
        BoundaryArray = (ct.c_double * self._numEstimatedParameters.value)
        self.ub = ct.pointer(BoundaryArray(*ub))
        self.lb = ct.pointer(BoundaryArray(*lb))
        self.startingValues = ct.pointer(BoundaryArray(*startingValues))

        # load the function for creating a SRES obj
        self.newSRES = self._loadNewSRES()

        self.getSolutionValues = self._loadGetSolutionValues()

        self.getTrace = self._loadGetTrace()

        # then call it, assigning the output pointer to this obj.
        # this SRES owns this pointer
        self._obj = self.newSRES(
            self._cost_function, self._popsize,
            self._numGenerations, self.startingValues,
            self.lb, self.ub, self._numEstimatedParameters, self._childrate
        )

    @staticmethod
    def callback(numEstimatedParameters: int):
        return ct.CFUNCTYPE(ct.c_double, ct.POINTER(ct.c_double * numEstimatedParameters))

    # SRES *newSRES(SRES::CostFunction cost, int populationSize, int numGenerations,
    #                   double *startingValues, const double *lb, double *ub);
    def _loadNewSRES(self):
        return self._sres._load_func(
            funcname="newSRES",
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

    def _loadGetSolutionValues(self):
        return self._sres._load_func(
            funcname="getSolutionValues",
            argtypes=[ct.c_int64],
            return_type=ct.POINTER(ct.c_double * self._numEstimatedParameters.value)
        )

    def _loadGetTrace(self):
        return self._sres._load_func(
            funcname="getTrace",
            argtypes=[ct.c_int64, ct.c_int32],
            return_type=ct.POINTER(ct.c_double*self._numGenerations.value)
        )

    _getLastError = _sres._load_func(
        funcname="getLastError",
        argtypes=[],
        return_type=ct.c_char_p  # pointer to char*
    )

    def getLastError(self):
        return self._getLastError()

    # https://stackoverflow.com/a/13452473/3059024
    _getBestValue = _sres._load_func(
        funcname="getBestValue",
        argtypes=[ct.c_int64],
        return_type=ct.POINTER(ct.c_double)
    )

    def getBestValue(self):
        return self._getBestValue(self._obj)

    _fit = _sres._load_func(
        funcname="fit",
        argtypes=[ct.c_int64],
        return_type=ct.c_bool
    )

    def fit(self):
        self._fit(self._obj)

        print([i for i in self.getTrace(self._obj, self._numGenerations).contents])

        dct = dict(
            best_cost=self.getBestValue().contents.value,
            trace=[i for i in self.getTrace(self._obj, self._numGenerations).contents],
            best_solution=[i for i in self.getSolutionValues(self._obj).contents]
        )
        return dct

    _freeSolutionValues = _sres._load_func(
        funcname="freeSolutionValues",
        argtypes=[ct.POINTER(ct.c_double)],
        return_type=None
    )

    _freeBestValue = _sres._load_func(
        funcname="freeBestValue",
        argtypes=[ct.POINTER(ct.c_double)],
        return_type=None
    )

    _setSeed = _sres._load_func(
        funcname="setSeed",
        argtypes=[ct.c_int64],
        return_type=None
    )

    _deleteSRES = _sres._load_func(
        funcname="deleteSRES",
        argtypes=[ct.c_int64],
        return_type=None
    )



    # def __del__(self):
    #     self._deleteSRES(self._obj)
