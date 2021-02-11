//
// Created by Ciaran on 05/02/2021.
//

#include "RandomNumberGenerator.h"
#include <random>
#include <iostream>

namespace opt {

    RandomNumberGenerator::RandomNumberGenerator(unsigned long long seed)
            : seed_(seed), generator_(std::default_random_engine(seed)) {}

    RandomNumberGenerator &RandomNumberGenerator::getInstance() {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        static RandomNumberGenerator singleton(seed);
        return singleton;
    }

    unsigned long long int RandomNumberGenerator::getSeed() const {
        return seed_;
    }

    void RandomNumberGenerator::setSeed(unsigned long long int seed) {
        // it is pointless setting a seed and not restarting the
        // rng with that seed. So setting the seed also resets the rng
        // with that seed
        seed_ = seed;
        generator_ = std::default_random_engine(seed_);
    }

//    std::default_random_engine RandomNumberGenerator::getGenerator() const {
//        return generator_;
//    }

    void RandomNumberGenerator::setGenerator(const std::default_random_engine &generator) {
        generator_ = generator;
    }

    double RandomNumberGenerator::uniformReal(double lb, double ub) {
        std::uniform_real_distribution<double> distibution(lb, ub);
        return distibution(generator_);
    }

    std::vector<double> RandomNumberGenerator::uniformReal(double lb, double ub, int size) {
        std::vector<double> r;
        for (int i = 0; i < size; i++) {
            r.push_back(uniformReal(lb, ub));
        }
        return r;
    }

    double RandomNumberGenerator::uniformInt(int lb, int ub) {
        std::uniform_int_distribution<int> distibution(lb, ub);
        return distibution(generator_);
    }

    std::vector<double> RandomNumberGenerator::uniformInt(int lb, int ub, int size) {
        std::vector<double> r;
        for (int i = 0; i < size; i++) {
            r.push_back(uniformInt(lb, ub));
        }
        return r;
    }


    double RandomNumberGenerator::normal(double mu, double sigma) {
        std::normal_distribution<double> distibution(mu, sigma);
        return distibution(generator_);
    }

    std::vector<double> RandomNumberGenerator::normal(double mu, double sigma, int size) {
        std::vector<double> r;
        for (int i = 0; i < size; i++) {
            r.push_back(normal(mu, sigma));
        }
        return r;
    }

    std::vector<std::vector<double>> RandomNumberGenerator::lhs(int npopulation, int nparams) {
        /*
         */
        // Each parameter range needs to be sampled uniformly. The scaled
        // parameter range ([0, 1)) needs to be split into
        // `npopulation` segments, each of which has the following
        // size:
        int i, j;
        double segsize = 1.0 / npopulation;

        // initialize empty npopulation x nparams matrix for samples and population
        std::vector<std::vector<double>> samples(npopulation, std::vector<double>(nparams));
        std::vector<std::vector<double>> population(npopulation, std::vector<double>(nparams));

        for (i = 0; i < npopulation; i++) {
            samples[i] = uniformReal(0, 1, nparams);
            // multiply by segsize
            for (j = 0; j < nparams; j++) {
                samples[i][j] *= segsize; // segsize is a constant number < 1
            }
        }
        // we now add to samples[i][j] an offset, to cover the range 0 to 1

        // create a linearly spaced vector to use as offsets
        // to do the scipy line:
        //      + np.linspace(0., 1., self.num_population_members,
        //                                 endpoint=False)[:, np.newaxis])
        std::vector<double> lin(npopulation);
        for (i = 0; i < npopulation; i++) {
            lin[i] = i / static_cast<double>(npopulation);
        }

        // we now add lin[j] to each column[i, j], shuffle and create our population matrix
        for (i = 0; i < npopulation; i++) {
            for (j = 0; j < nparams; j++) {
                // first scale the samples so they are in range 0-1 down each column
                samples[i][j] += lin[i];
            }
        }

        // transpose our matrix
        std::vector<std::vector<double>> tmp_(nparams, std::vector<double>(npopulation));
        for (i = 0; i < npopulation; i++) {
            for (j = 0; j < nparams; j++) {
                tmp_[j][i] = samples[i][j];
            }
        }

        // now we can shuffle the rows of the transposed matrix
        for (j = 0; j < nparams; j++) {
            // create column vectors with randomized indices, one for each column
            std::shuffle(tmp_[j].begin(), tmp_[j].end(), generator_);
        }

        // and we transpose back
        for (i = 0; i < nparams; i++) {
            for (j = 0; j < npopulation; j++) {
                population[j][i] = tmp_[i][j];
            }
        }

        return population;


    }

    std::vector<std::vector<double>> RandomNumberGenerator::lhs(
            int npopulation, int nparams, std::vector<double> lb, std::vector<double> ub, bool sampleInLogspace) {
        int i, j;

//        if (sampleInLogspace) {
//            for (j = 0; j < nparams; j++) {
//                lb[j] = log10(lb[j]);
//                ub[j] = log10(ub[j]);
//            }
//        }

        // initialize empty npopulation x nparams matrix for samples and population
        std::vector<std::vector<double>> samples(npopulation, std::vector<double>(nparams));
        std::vector<std::vector<double>> population(npopulation, std::vector<double>(nparams));

        double step;
        for (i = 1; i < npopulation + 1; i++) {
            // multiply by segsize
            for (j = 0; j < nparams; j++) {
                step = (ub[j] - lb[j]) / npopulation;
                samples[i - 1][j] = uniformReal(lb[j] + i * step, lb[j] + (i + 1) * step);
                population[i - 1][j] = samples[i - 1][j];
            }
        }

        // transpose our matrix for easy shuffling along the now rows but soon to be columns
        std::vector<std::vector<double>> tmp_(nparams, std::vector<double>(npopulation));
        for (i = 0; i < npopulation; i++) {
            for (j = 0; j < nparams; j++) {
                tmp_[j][i] = samples[i][j];
            }
        }

        // now we can shuffle the rows of the transposed matrix
        for (j = 0; j < nparams; j++) {
            // create column vectors with randomized indices, one for each column
            std::shuffle(tmp_[j].begin(), tmp_[j].end(), generator_);
        }

        // and we transpose back, multiplying out the log10 if needed
        for (i = 0; i < nparams; i++) {
            for (j = 0; j < npopulation; j++) {
//                if (sampleInLogspace)
//                    population[j][i] = pow(10, tmp_[i][j]);
//                else
                    population[j][i] = tmp_[i][j];
            }
        }
        return population;
    }

}