import unittest
from sres import SRES

def beale(position):
    """
    optimum at (3.0, 0.5) = 0
    :param position:
    :return:
    """
    x, y = position
    # x = 10**x
    # y = 10**y
    return (1.5 - x + x * y) ** 2 + (2.25 - x + x * y ** 2) ** 2 + (2.625 - x + x * y ** 3) ** 2


@SRES.callback(2)
def cost_fun(parameters):
    return beale([parameters.contents[0], parameters.contents[1]])


class SRESTests(unittest.TestCase):

    def setUp(self) -> None:
        ngen = 250
        popsize = 10
        self.sres = SRES(
            cost_fun, popsize=popsize, numGenerations=ngen,
            startingValues=[8.324, 7.335],
            lb=[0.1, 0.1],
            ub=[10, 10],
            childrate=7, logspace=True

        )
        self.sres.setSeed(4)

    def tearDown(self) -> None:
        pass

    def test_best_value(self):
        """TEST_F(CCSRESTests, TestGetBestValue) {
            SRES *sres = SRES_newSRES(cost, 10, 50, s, l, u, 2, 7);
            SRES_setSeed(sres, 4);
            SRES_fit(sres);
            double best;
            SRES_getBestFitnessValue(sres, &best);
            double x = 8.0722599999999995e-11;
            ASSERT_NEAR(x, best, 0.001);
            SRES_deleteSRES(sres);
        }"""
        results = self.sres.fit()
        self.assertAlmostEqual(1.0056368571769424e-07, results["bestFitness"], places=5)

    def testSizeOfHOF(self):
        self.sres.fit()
        self.assertEqual(18, self.sres._getSizeOfHallOfFame(self.sres._obj))

    def test_hof(self):
        self.sres.fit()
        hof = self.sres.getHallOfFame()
        alwaysDecreasing = True
        for i in range(1, len(hof)):
            if hof[i] > hof[i-1]:
                alwaysDecreasing = False
                break
        self.assertTrue(alwaysDecreasing)

    def test_getSol(self):
        self.sres.fit()
        self.assertAlmostEqual(3, self.sres.getSolution()[0], places=2)
        self.assertAlmostEqual(0.5, self.sres.getSolution()[1], places=2)


if __name__ == '__main__':
    unittest.main()
