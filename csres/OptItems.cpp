//
// Created by Ciaran on 05/02/2021.
//

#include <algorithm>
#include "OptItems.h"
#include "Error.h"

namespace csres {

    OptItems::OptItems(std::vector<OptItem> optItems)
            : optItems_(std::move(optItems)) {}

    [[noreturn]] OptItems::OptItems(const std::vector<double> &startingValues, const std::vector<double> &lb,
                       const std::vector<double> &ub) {

        std::vector<int> sizes(
                {static_cast<int>(startingValues.size()),
                 static_cast<int>(lb.size()),
                 static_cast<int>(ub.size())}
        );
        bool equal = false;
        if (std::adjacent_find(sizes.begin(), sizes.end(), std::not_equal_to<>()) == sizes.end()) {
            equal = true;
        }
        if (!equal){
            LOGIC_ERROR << "Input vectors are not equal sizes. The startingValues vector is "
                           << startingValues.size() << "; the lb vector is: " << lb.size()
                           << "; and the ub vector is " << ub.size() << std::endl;
        }
        for (int i=0; i<startingValues.size(); i++){
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

