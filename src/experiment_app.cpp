//
// Created by Xiuge Chen on 5/25/20.
//

#include <spdlog/spdlog.h>

#include "experiment_app.h"

namespace Xiuge::RangeTree {

namespace {

const uint32_t N = 1000000;
const uint32_t NUM_REPEAT = 100;

}

void ExperimentApp::construct_time_data_length(const std::vector<uint32_t>& dataLens){
    spdlog::info("Start construction time test with various data length");

    mDataGenerator.set_range(1, N);

    for (auto len: dataLens) {
        spdlog::info("Start with data length={}", len);

        auto vec = mDataGenerator.generate_point_set(len);

        // Test on naive construction algorithm
        std::vector<Point> naive_copy{vec};
        OrgRangeTree orgRangeTreeNaive;

        long long int startTime = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();

        orgRangeTreeNaive.construct_tree(naive_copy, true);

        long long int endTime = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();

        spdlog::info("[ExperimentApp] Finish construction time testing on Original Range Tree with naive construction algorithm and data"
                     "length={}, running time={}", len, endTime - startTime);

        // Test on smart construction algorithm
        std::vector<Point> smart_copy{vec};
        OrgRangeTree orgRangeTreeSmart;

        startTime = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();

        orgRangeTreeSmart.construct_tree(smart_copy, false);

        endTime = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();

        spdlog::info("[ExperimentApp] Finish construction time testing on Original Range Tree with smart construction algorithm and data"
                     "length={}, running time={}", len, endTime - startTime);
    }
}

void ExperimentApp::query_time_data_length(const std::vector<uint32_t>& dataLens){
    spdlog::info("Start query time test with various data length");

    mDataGenerator.set_range(1, N);

    for (auto len: dataLens) {
        spdlog::info("Start with data length={}", len);

        auto range = static_cast<uint32_t>(0.05 * N);

        auto dataVec = mDataGenerator.generate_point_set(len);
        std::vector<Query> queryVec;
        for (unsigned int i = 0; i < NUM_REPEAT; ++i) {
            queryVec.emplace_back(mDataGenerator.generate_a_query(range));
        }

        // Test on original range tree
        OrgRangeTree orgRangeTree;
        orgRangeTree.construct_tree(dataVec, false);

        long long int sum_time = 0;
        unsigned long long int sum_k = 0;

        for (unsigned int i = 0; i < NUM_REPEAT; ++i) {
            long long int startTime = std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count();

            std::vector<Point> orgResult;
            orgRangeTree.report_points(queryVec[i], orgResult);

            long long int endTime = std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count();

            sum_time = sum_time + (endTime - startTime);
            sum_k = sum_k + orgResult.size();
        }

        spdlog::info("[ExperimentApp] Finish query time testing on Original Range Tree with data"
                     "length={}, range={}, k={}, running time={}", len, range, sum_k / NUM_REPEAT,  sum_time / NUM_REPEAT);

        // Test on smart construction algorithm
        FcRangeTree fcRangeTree;
        fcRangeTree.construct_tree(dataVec, false);

        sum_time = 0;
        sum_k = 0;

        for (unsigned int i = 0; i < NUM_REPEAT; ++i) {
            long long int startTime = std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count();

            std::vector<Point> fcResult;
            fcRangeTree.report_points(queryVec[i], fcResult);

            long long int endTime = std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count();

            sum_time = sum_time + (endTime - startTime);
            sum_k = sum_k + fcResult.size();
        }

        spdlog::info("[ExperimentApp] Finish query time testing on Fractional Cascading Range Tree with data"
                     "length={}, range={}, k={}, running time={}", len, range, sum_k / NUM_REPEAT, sum_time / NUM_REPEAT);
    }
}

void ExperimentApp::query_time_query_range(const std::vector<double>& queryRangePers){
    spdlog::info("Start query time test with various query range");

    mDataGenerator.set_range(1, N);
    auto dataVec = mDataGenerator.generate_point_set(N);

    OrgRangeTree orgRangeTree;
    orgRangeTree.construct_tree(dataVec, false);

    FcRangeTree fcRangeTree;
    fcRangeTree.construct_tree(dataVec, false);

    for (auto rangePer: queryRangePers) {
        auto range = static_cast<uint32_t>(rangePer * N);
        spdlog::info("Start with query range={}", range);

        std::vector<Query> queryVec;
        for (unsigned int i = 0; i < NUM_REPEAT; ++i) {
            queryVec.emplace_back(mDataGenerator.generate_a_query(range));
        }

        // Test on original range tree
        long long int sum_time = 0;
        unsigned long long int sum_k = 0;

        for (unsigned int i = 0; i < NUM_REPEAT; ++i) {
            long long int startTime = std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count();

            std::vector<Point> orgResult;
            orgRangeTree.report_points(queryVec[i], orgResult);

            long long int endTime = std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count();

            sum_time = sum_time + (endTime - startTime);
            sum_k = sum_k + orgResult.size();
        }

        spdlog::info("[ExperimentApp] Finish query time testing on Original Range Tree with data"
                     "length={}, range={}, k={}, running time={}", N, range,  sum_k / NUM_REPEAT, sum_time / NUM_REPEAT);

        // Test on smart construction algorithm
        sum_time = 0;
        sum_k = 0;

        for (unsigned int i = 0; i < NUM_REPEAT; ++i) {
            long long int startTime = std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count();

            std::vector<Point> fcResult;
            fcRangeTree.report_points(queryVec[i], fcResult);

            long long int endTime = std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count();

            sum_time = sum_time + (endTime - startTime);
            sum_k = sum_k + fcResult.size();
        }

        spdlog::info("[ExperimentApp] Finish query time testing on Fractional Cascading Range Tree with data"
                     "length={}, range={}, k={}, running time={}", N, range,  sum_k / NUM_REPEAT, sum_time / NUM_REPEAT);
    }
}

} // namespace ::Xiuge::RangeTree