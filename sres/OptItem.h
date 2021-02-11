//
// Created by Ciaran on 05/02/2021.
//

#ifndef SRES_OPTITEM_H
#define SRES_OPTITEM_H

#include <vector>

namespace opt {

    class OptItem {

    public:

        OptItem() = default;

        OptItem(double startingValue, double lb, double ub, bool logspace);

        [[nodiscard]] double getLb() const;

        void setLb(double lb);

        [[nodiscard]] double getUb() const;

        void setUb(double ub);

        [[nodiscard]] double getValue() const;

        void setValue(double startingValue);

        [[nodiscard]] double getStartingValue() const;

        void setStartingValue(double startingValue);

        [[nodiscard]] int checkConstraint() const;

        [[nodiscard]] int checkConstraint(const double &value) const;

        [[nodiscard]] bool checkLowerBound(const double &value) const;

        [[nodiscard]] bool checkUpperBound(const double &value) const;

        /**
         * @brief scale a number between 0 and 1 to parameter value
         */
        [[nodiscard]] double scaleParameter(double trial) const;

        /**
         * @brief unscale a number from parameters to 0 and 1
         */
        [[nodiscard]] double unscaleParameter(double parameter) const;

    private:
        void computeScaleArg1();

        void computeScaleArg2();


        /**
         * @brief lower boundary of parameter during optimization
         */
        double lb_;

        /**
         * @brief upper boundary of parameter during optimization
         */
        double ub_;

        /**
         * @brief current value of the optimization item in parameter space
         */
        double value_;


        /**
         * @brief initial parameter value
         */
        double startingValue_;

        /**
         * @brief scaled value of optimization item. Experimental
         */
        double scaleArg1_;

        /**
         * @brief scaled value of optimization item. Experimental
         */
        double scaleArg2_;

        double scaledParameter_;
    };
}


#endif //SRES_OPTITEM_H
