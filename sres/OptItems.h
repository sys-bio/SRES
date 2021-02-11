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

        OptItems(const std::vector<double> &startingValues, const std::vector<double> &lb,
                 const std::vector<double> &ub, bool logspace);

        int size();

        std::vector<OptItem>::iterator begin();

        std::vector<OptItem>::iterator end();

        OptItem &operator[](int index);

        [[nodiscard]] const std::vector<OptItem> &getOptItems() const;

        void setOptItems(const std::vector<OptItem> &optItems);

        [[nodiscard]] const std::vector<double> &getLb() const;

        void setLb(const std::vector<double> &lb);

        [[nodiscard]] const std::vector<double> &getUb() const;

        void setUb(const std::vector<double> &ub);

        [[nodiscard]] const std::vector<double> &getStartingValues() const;

        void setStartingValues(const std::vector<double> &startingValues);

    private:

        std::vector<OptItem> optItems_;

        std::vector<double> lb_;

        std::vector<double> ub_;

        std::vector<double> starting_values_;

    };
}


#endif //SRES_OPTITEMS_H
