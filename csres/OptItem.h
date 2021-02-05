//
// Created by Ciaran on 05/02/2021.
//

#ifndef SRES_OPTITEM_H
#define SRES_OPTITEM_H

#include <vector>

namespace csres {

    class OptItem {

    public:

        OptItem() = default;

        OptItem(double startingValue, double lb, double ub);

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

    private:

        double lb_;

        double ub_;

        double value_;

        double startingValue_;

    };
}


#endif //SRES_OPTITEM_H
