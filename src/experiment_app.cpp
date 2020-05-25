//
// Created by Xiuge Chen on 5/25/20.
//

#include <spdlog/spdlog.h>

#include "experiment_app.h"

namespace Xiuge::RangeTree {

void ExperimentApp::construct_time_data_length(const std::vector<uint32_t>& dataLens){
    spdlog::info("Start construction time test with various data length");

    mDataGenerator.set_range(1, 1000000);

    for (auto len: dataLens) {
        spdlog::info("Start with data length={}", len);

        auto vec = mDataGenerator.generate_point_set(len);

        // Test on naive construction algorithm
        std::vector<Point> naive_copy{vec};
        OrgRangeTree mOrgRangeTreeNaive;

        long long int startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
        ).count();

        mOrgRangeTreeNaive.construct_tree(naive_copy, true);

        long long int endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
        ).count();

        spdlog::info("[ExperimentApp] Finish construction time testing on Original Range Tree with naive construction algorithm and data"
                     "length={}, running time={}", len, endTime - startTime);

        // Test on smart construction algorithm
        std::vector<Point> smart_copy{vec};
        OrgRangeTree mOrgRangeTreeSmart;

        startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
        ).count();

        mOrgRangeTreeSmart.construct_tree(smart_copy, false);

        endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
        ).count();

        spdlog::info("[ExperimentApp] Finish construction time testing on Original Range Tree with smart construction algorithm and data"
                     "length={}, running time={}", len, endTime - startTime);
    }
}

void ExperimentApp::query_time_query_range(){

}

void ExperimentApp::query_time_data_length(){

}

} // namespace ::Xiuge::RangeTree