from abc import ABC, abstractmethod
from typing import List
import ctypes as ct
import os, sys


class _SRESLoader:

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
            f"{self._get_shared_library_prefix()}SRES{self._get_shared_library_extension()}"
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
    _sres = _SRESLoader()

    # decorator for wrapping cost function written in Python
    COST_FUNCTION_CALLBACK = ct.CFUNCTYPE(None, ct.POINTER(ct.c_double * 2), ct.POINTER(ct.c_double),
                                          ct.POINTER(ct.c_double))

    # note when seed is 0, it'll be randomized. Special case.
    def __init__(self, cost_function: COST_FUNCTION_CALLBACK, ngen: int, ub: List[float], lb: List[float],
                 parent_popsize: int = 50, child_popsize: int = 200, es: int = 1,
                 gamma: float = 0.85, alpha: float = 0.2, pf: float = 0.45,
                 varphi: float = 1.0, retry: int = 1, seed: int = 0):
        self.cost_function = cost_function
        self.dim = ct.c_int32(len(ub))
        self.seed = ct.c_int32(seed)
        self.es = ct.c_int32(es)
        BoundaryArray = (ct.c_double * self.dim.value)
        self.ub = ct.pointer(BoundaryArray(*ub))
        self.lb = ct.pointer(BoundaryArray(*lb))
        self.parent_popsize = ct.c_int32(parent_popsize)
        self.constraint = ct.c_int32(0)
        self.child_popsize = ct.c_int32(child_popsize)
        self.ngen = ct.c_int32(ngen)
        self.gamma = ct.c_double(gamma)
        self.alpha = ct.c_double(alpha)
        self.pf = pf
        self.varphi = ct.c_double(varphi)
        self.retry = ct.c_int32(retry)

        # Does nothing for now: just a plpaceholder
        self._trsfm = self._makeTransformFun(self.dim)

        # ptr to ESParameter**
        self._param_ptr = self._makeESParameter()

        # ptr to ESPopulation**
        self._pop_ptr = self._makeESPopulation()

        # ptr to ESStatistics**
        self._stat_ptr = self._makeESStatistics()

        # load the ESInitial function then call it with input parameters
        self._loadESInitial()(
            self.seed,
            self._param_ptr,
            self._trsfm,
            self.cost_function,
            self.es,
            self.constraint,
            self.dim,
            self.ub,
            self.lb,
            self.parent_popsize,
            self.child_popsize,
            self.ngen,
            self.gamma,
            self.alpha,
            self.varphi,
            self.retry,
            self._pop_ptr,
            self._stat_ptr
        )

    @staticmethod
    @COST_FUNCTION_CALLBACK
    def cost_function(x, f, g):
        raise NotImplementedError("You need to implement a cost function")

    def __del__(self):
        self._freeTransformFun(self._trsfm)
        self._ESDeInitial(self._param_ptr, self._pop_ptr, self._stat_ptr)

    def fit(self, printStats: bool = False):
        for i in range(self.ngen.value):
            self.step(self.pf, printStats)

    def step(self, pf: float, printStats=False) -> None:
        return self._ESStep(self._pop_ptr, self._param_ptr, self._stat_ptr, pf, printStats)

    def _makeDoubleArrayPtr(self, input: List[float]):
        """returns a ctypes double array from input"""
        ctypes_double_type = ct.c_double * len(input)
        my_double_arr = ctypes_double_type(*input)
        return ct.pointer(my_double_arr)

    # ESParameter **makeESParameter();
    _makeESParameter = _sres._load_func(
        funcname="makeESParameter",
        argtypes=[],  # void
        return_type=ct.c_uint64  # return type: ESParameter**
    )

    # trsfm can be a decorator too. For now its just ignored
    # ESfcnTrsfm * makeTransformFun()
    _makeTransformFun = _sres._load_func(
        funcname="makeTransformFun",
        argtypes=[ct.c_int32],
        return_type=ct.c_uint64
    )

    # void freeTransformFun(ESfcnTrsfm *fun)
    _freeTransformFun = _sres._load_func(
        funcname="freeTransformFun",
        argtypes=[ct.c_int64],
        return_type=None,
    )

    """
    /*********************************************************************
     ** initialize: parameters,populations and random seed              **
     ** ESInitial(seed, param,trsfm, fg,es,constraint,dim,ub,lb,miu,    **
     **            lambda,gen, gamma, alpha, varphi, retry,             **
     **             population, stats)                                  **
     ** seed: random seed, usually esDefSeed=0 (pid*time)               **
     ** outseed: seed value assigned , for next use                     **
     ** param: point to parameter                                       **
     ** fg: functions of fitness and constraints                        **
     ** trsfm: to transform sp/op                                       **
     ** es: ES process, esDefESPlus/esDefESSlash                        **
     ** constraint: number of constraints                               **
     ** dim: dimension/number of genes in genome                        **
     ** ub[dim]: up bounds                                              **
     ** lb[dim]: low bounds                                             **
     ** miu: parent/population size                                     **
     ** lambda: offsping/population size                                **
     ** gen: number of generations                                      **
     ** gamma: usually esDefGamma=0.85                                  **
     ** alpha: usually esDefAlpha=0.2                                   **
     ** chi: chi = 1/2n +1/2sqrt(n)                                     **
     ** varphi: = sqrt((2/chi)*log((1/alpha)*(exp(varphi^2*chi/2)       **
     **                  -(1-alpha))))                                  **
     **         expected rate of convergence                            **
     ** retry: retry times to check bounds                              **
     ** tau: learning rates: tau = varphi/(sqrt(2*sqrt(dim)))           **
     ** tar_: learning rates: tau_ = varphi((sqrt(2*dim)                **
     ** population: point to this population                            **
     ** stats: point to statistics                                      **
     **                                                                 **
     ** ESDeInitial(param,populationi,stats)                            **
     ** free param and population                                       **
     *********************************************************************/
    """

    # void ESInitial(unsigned int, ESParameter **, ESfcnTrsfm *, \
    #                ESfcnFG*, int, int, int, double *, double *, int, int, int, \
    #                double, double, double, int, \
    #                ESPopulation **, ESStatistics **);

    def _loadESInitial(self):
        """
        The dimensions of the optimization problem are not known
        before loading SRES so this must be done in a method.
        :param dim:
        :return:
        """
        return self._sres._load_func(
            funcname="ESInitial",
            argtypes=[
                ct.c_int32,  # unsigned int seed,
                ct.c_int64,  # ESParameter **param,
                ct.c_int64,  # ESfcnTrsfm *trsfm,
                self.COST_FUNCTION_CALLBACK,  # ESfcnFG fg
                ct.c_int32,  # int es, 0 or 1; ES process, esDefESPlus/esDefESSlash
                ct.c_int32,  # int constraint,
                ct.c_int32,  # int dim,
                ct.POINTER(ct.c_double * self.dim.value),
                ct.POINTER(ct.c_double * self.dim.value),
                ct.c_int32,  # int miu,
                ct.c_int32,  # int lambda,
                ct.c_int32,  # int gen,
                ct.c_double,  # double gamma,
                ct.c_double,  # double alpha,
                ct.c_double,  # double varphi,
                ct.c_int32,  # int retry,
                ct.c_int64,  # ESPopulation **population
                ct.c_int64,  # ESStatistics **stats
            ],
            return_type=None)

    def getBestParameters(self) :
        _ESGetBestParameters = self._sres._load_func(
            funcname="ESGetBestParameters",
            argtypes=[
                ct.c_int64  # ESStatistics**
            ],
            return_type=ct.POINTER(ct.c_double * self.dim.value)
        )
        dbl_array = _ESGetBestParameters(self._stat_ptr)
        return [x for x in dbl_array.contents]


    """/**
     * @brief Create a ESPopulation 
     * @details heap allocated. User free's with freeESPopulation
     */
     """
    # ESPopulation *makeESPopulation();
    _makeESPopulation = _sres._load_func(
        funcname="makeESPopulation",
        argtypes=[],
        return_type=ct.c_uint64  # ESPopulation *
    )

    # ESStatistics *makeESStatistics();
    _makeESStatistics = _sres._load_func(
        funcname="makeESStatistics",
        argtypes=[],
        return_type=ct.c_uint64  # ESPopulation *
    )

    # void ESDeInitial(ESParameter *, ESPopulation *, ESStatistics *);
    _ESDeInitial = _sres._load_func(
        funcname="ESDeInitial",
        argtypes=[
            ct.c_uint64,  # ESParameter *,
            ct.c_uint64,  # ESPopulation *,
            ct.c_uint64,  # ESStatistics *
        ],
        return_type=None
    )

    """
    /*********************************************************************
     ** stepwise evolution                                              **
     ** ESStep(population, param, stats, pf)                            **
     **                                                                 **
     ** -> Stochastic ranking -> sort population based on ranking index **
     ** -> Mutate (recalculate f/g/phi) -> do statistics analysis on    **
     ** this generation -> print statistics information                 **
     *********************************************************************/
    """
    # void ESStep(ESPopulation *, ESParameter *, ESStatistics *, double, bool);
    _ESStep = _sres._load_func(
        funcname="ESStep",
        argtypes=[
            ct.c_int64,  # ESPopulation *,
            ct.c_int64,  # ESParameter  *,
            ct.c_int64,  # ESStatistics *,
            ct.c_double,  # double
            ct.c_bool,  # print stats
        ],
        return_type=ct.c_double)


