//
// Created by Ciaran on 05/02/2021.
//

#include <algorithm>
#include "OptItems.h"
#include "Error.h"

namespace csres {

    OptItems::OptItems(std::vector<OptItem> optItems)
            : optItems_(std::move(optItems)) {}

    OptItems::OptItems(const std::vector<double> &startingValues, const std::vector<double> &lb,
                       const std::vector<double> &ub) {

        int s1 = startingValues.size();
        int s2 = lb.size();
        int s3 = ub.size();
        std::vector<int> sizes({s1, s2, s3});
        bool equal = false;
        if (std::adjacent_find(sizes.begin(), sizes.end(), std::not_equal_to<>()) == sizes.end()) {
            equal = true;
        }
        if (!equal) {
            LOGIC_ERROR << "Input vectors are not equal sizes. The startingValues vector is "
                        << s1 << "; the lb vector is: " << s2
                        << "; and the ub vector is " << s3 << std::endl;
        }
        for (int i = 0; i < startingValues.size(); i++) {
            optItems_.emplace_back(startingValues[i], lb[i], ub[i]);
        }
    }

    OptItems::OptItems(std::initializer_list<OptItem> optItems)
            : optItems_(std::vector<OptItem>(optItems.begin(), optItems.end())) {}

    int OptItems::size() {
        return optItems_.size();
    }

    std::vector<OptItem>::iterator OptItems::begin() {
        return optItems_.begin();
    }

    std::vector<OptItem>::iterator OptItems::end() {
        return optItems_.end();
    }

    OptItem &OptItems::operator[](int index) {
        return optItems_[index];
    }

}

