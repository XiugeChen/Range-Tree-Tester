//
// Created by Xiuge Chen on 5/25/20.
//

#ifndef RANGETREE_EXPERIMENT_APP_H
#define RANGETREE_EXPERIMENT_APP_H

#include "data_generator.h"
#include "org_range_tree.h"
#include "fc_range_tree.h"

namespace Xiuge::RangeTree {

/**
 * The experiment
 */
class ExperimentApp {
public:
    void construct_time_data_length(const std::vector<uint32_t>& dataLens);

    void query_time_query_range();

    void query_time_data_length();

private:
    DataGenerator mDataGenerator;
};

} // namespace ::Xiuge::RangeTree

#endif //RANGETREE_EXPERIMENT_APP_H
