import unittest

from ..sres import SRES
import ctypes as ct

from sres import SRES
import numpy as np
import ctypes as ct


def generateData(mu, sigma):
    return np.random.normal(mu, sigma, 10)


EXP_DATA = generateData(5, 0.1)



@SRES.COST_FUNCTION_CALLBACK
def cost_fun(x, f, g):
    sim = generateData(x.contents[0], x.contents[1])
    cost = 0
    for i in range(10):
        cost += (EXP_DATA[i] - sim[i]) ** 2
    f.contents.value = cost



class Test(unittest.TestCase):

    def setUp(self) -> None:
        self.sres = SRES(ngen=100, lb=[0.01] * 2, ub=[10] * 2, seed=1234)

    def test_makeESParameter(self):
        cptr = self.sres._makeESParameter()
        self.assertIsNotNone(cptr)
        # free the pointer
        self.sres._freeESParameter(cptr)

    def test_makeESStatistics(self):
        cptr = self.sres._makeESStatistics()
        self.assertIsNotNone(cptr)
        self.sres._freeESStatistics(cptr)

    def test_makeESPopulation(self):
        cptr = self.sres._makeESPopulation()
        self.assertIsNotNone(cptr)
        self.sres._freeESPopulation(cptr)

    def test_makeCostFunPtr(self):
        cptr = self.sres.getCostFunPtr()
        self.assertIsNotNone(cptr)
        self.sres.freeCostFunPtr(cptr)

    def test_how_to_make_array_using_ctypes(self):
        DoubleArrayLen2 = ct.c_double * 2
        print(DoubleArrayLen2)
        arr = DoubleArrayLen2(*[0.1, 0.1])
        print(arr)

    def test_ESInitialWithPtrFitnessFcn(self):
        esparam = self.sres._makeESParameter()
        stats = self.sres._makeESStatistics()
        pop = self.sres._makeESPopulation()
        costFun = self.sres.getCostFunPtr()
        trsf = self.sres._getTransformFun()

        # https://stackoverflow.com/questions/51131433/how-to-pass-lists-into-a-ctypes-function-on-python/51132594
        DoubleArrayLen2 = ct.c_double * 2

        seed = 0
        gamma = 0.85
        alpha = 0.2
        varalphi = 1
        retry = 10
        es = 0
        miu = 30
        lamb = 200
        gen = 1750

        ptr = self.sres.ESInitialWithPtrFitnessFcn(
            seed,  # unsigned int seed,
            esparam,  # ESParameter **param,
            trsf,  # ESfcnTrsfm *trsfm,
            costFun,  # ESfcnFG* fg,
            es,  # int es,
            0,  # int constraint,
            2,  # int dim,
            DoubleArrayLen2(10.0, 10.0),  # double *ub,
            DoubleArrayLen2(0.1, 0.1),  # double *lb,
            miu,  # int miu,
            lamb,  # int lambda,
            gen,  # int gen,
            gamma,  # double gamma,
            alpha,  # double alpha,
            varalphi,  # double varphi,
            retry,  # int retry,
            stats,  # ESPopulation **population
            pop,  # ESStatistics **stats
        )

        self.sres._freeESParameter(esparam)
        self.sres._freeESStatistics(stats)
        self.sres._freeESPopulation(pop)
        self.sres.freeCostFunPtr(costFun)
        self.sres.freeTransformFun(trsf)

    def test(self):
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
        import tellurium as te
        model = te.loada(MODEL)
        print(model.getGlobalParameterIds())

    # def test_makeIndividual(self):
    #     self.sres.makeIndividual()
    #
    # def test_freeIndividual(self):
    #     self.sres.freeIndividual()
    #
    # def test_makePopulation(self):
    #     self.sres.makePopulation()
    #
    # def test_freePopulation(self):
    #     self.sres.freePopulation()
    #
    # def test_do_nothing_transform(self):
    #     self.sres.do_nothing_transform()
    #
    # def test_rss_cost(self):
    #     self.sres.rss_cost()
    #
    # def test_ESDeInitial(self):
    #     self.sres.ESDeInitial()
    #
    # def test_ESDeInitialPopulation(self):
    #     self.sres.ESDeInitialPopulation()
    #
    # def test_ESInitialIndividual(self):
    #     self.sres.ESInitialIndividual()
    #
    # def test_ESDeInitialIndividual(self):
    #     self.sres.ESDeInitialIndividual()
    #
    # def test_ESPrintIndividual(self):
    #     self.sres.ESPrintIndividual()
    #
    # def test_ESPrintOp(self):
    #     self.sres.ESPrintOp()
    #
    # def test_ESPrintSp(self):
    #     self.sres.ESPrintSp()
    #
    # def test_ESCopyIndividual(self):
    #     self.sres.ESCopyIndividual()
    #
    # def test_ESInitialStat(self):
    #     self.sres.ESInitialStat()
    #
    # def test_ESDeInitialStat(self):
    #     self.sres.ESDeInitialStat()
    #
    # def test_ESDoStat(self):
    #     self.sres.ESDoStat()
    #
    # def test_ESPrintStat(self):
    #     self.sres.ESPrintStat()
    #
    # def test_ESStep(self):
    #     self.sres.ESStep()
    #
    # def test_ESSortPopulation(self):
    #     self.sres.ESSortPopulation()
    #
    # def test_ESSelectPopulation(self):
    #     self.sres.ESSelectPopulation()
    #
    # def test_ESMutate(self):
    #     self.sres.ESMutate()
    #
    #
    #
