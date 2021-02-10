//
// Created by Ciaran on 05/02/2021.
//

#include "OptItem.h"


namespace opt {

    OptItem::OptItem(double startingValue, double lb, double ub)
            : value_(startingValue), startingValue_(value_), lb_(lb), ub_(ub),
            scaledParameter_(unscaleParameter(value_)){}

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

    void OptItem::computeScaleArg1() {
        scaleArg1_ = 0.5 * (lb_ + ub_);
    }

    void OptItem::computeScaleArg2() {
        scaleArg2_ = abs(lb_ - ub_);
    }

    /**
     * @brief scale a number between 0 and 1 to parameter value
     */
    double OptItem::scaleParameter(double trial) const{
        // self.__scale_arg1 + (trial - 0.5) * self.__scale_arg2
        return scaleArg1_ + (trial + 0.5) * scaleArg2_;
    }

    /**
     * @brief unscale a number from parameters to 0 and 1
     */
    double OptItem::unscaleParameter(double parameterValue) const{
        return (parameterValue - scaleArg1_) / scaleArg2_ + 0.5;
    }


}