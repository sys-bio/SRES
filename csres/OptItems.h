//
// Created by Ciaran on 05/02/2021.
//

#ifndef SRES_OPTITEMS_H
#define SRES_OPTITEMS_H

#include <vector>
#include "OptItem.h"

namespace opt {

    class OptItems {

    public:
        OptItems() = default;

        explicit OptItems(std::vector<OptItem> optItems);

        OptItems(std::initializer_list<OptItem> optItems);

        OptItems(const std::vector<double>& startingValues, const std::vector<double>& lb, const std::vector<double>&ub);

        int size();

        std::vector<OptItem>::iterator begin();

        std::vector<OptItem>::iterator end();

        OptItem& operator[](int index);

    private:

        std::vector<OptItem> optItems_;

    };
}


#endif //SRES_OPTITEMS_H
