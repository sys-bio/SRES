//
// Created by Ciaran on 05/02/2021.
//

#include "OptItem.h"


namespace opt {

    OptItem::OptItem(double startingValue, double lb, double ub)
            : value_(startingValue), startingValue_(value_), lb_(lb), ub_(ub) {}

    double OptItem::getLb() const {
        return lb_;
    }

    void OptItem::setLb(double lb) {
        lb_ = lb;
    }

    double OptItem::getUb() const {
        return ub_;
    }

    void OptItem::setUb(double ub) {
        ub_ = ub;
    }

    double OptItem::getValue() const {
        return value_;
    }

    void OptItem::setValue(double startingValue) {
        value_ = startingValue;
    }

    int OptItem::checkConstraint() const {
        if (lb_ > getValue()) return -1;

        if (getValue() > ub_) return 1;

        return 0;
    }

    int OptItem::checkConstraint(const double &value) const {
        if (lb_ > value) return -1;

        if (value > ub_) return 1;

        return 0;
    }

    double OptItem::getStartingValue() const {
        return startingValue_;
    }

    void OptItem::setStartingValue(double startingValue) {
        startingValue_ = startingValue;
    }


    bool OptItem::checkLowerBound(const double &value) const {
        return lb_ <= value;
    }

    bool OptItem::checkUpperBound(const double &value) const {
        return value <= ub_;
    }
}