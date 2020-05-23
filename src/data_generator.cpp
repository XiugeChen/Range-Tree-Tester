//
// Created by Xiuge Chen on 5/21/20.
//

#include <spdlog/spdlog.h>

#include "data_generator.h"
#include "utils.h"

namespace Xiuge::RangeTree {

DataGenerator::DataGenerator()
    : mGenerator( std::mt19937(mRd()))
    , mPtDist( std::uniform_int_distribution<uint32_t>(0, 1) )
{}

void DataGenerator::set_range(const uint32_t coord_min, const uint32_t coord_max) {
    if (unlikely(coord_max < coord_min))
        throw std::runtime_error("[DataGenerator] set range with upper bound smaller than lower bound");

    decltype(mPtDist.param()) new_range (coord_min, coord_max);
    mPtDist.param(new_range);
    spdlog::debug("Set the range of future point to be: min={}, max={}", coord_min, coord_max);
}

std::vector<Point> DataGenerator::generate_point_set(const uint32_t n) {
    std::vector<Point> points;

    for (uint32_t i = 1; i <= n; ++i) {
        Point pt = generate_a_point();
        pt.id = i;
        points.emplace_back(pt);
    }

    return points;
}

Query DataGenerator::generate_a_query(const uint32_t range) {
    uint32_t old_min = mPtDist.min(), old_max = mPtDist.max();

    if (unlikely(old_max - old_min < range))
        throw std::runtime_error("[DataGenerator] query range specified greater than point range");

    uint32_t new_max = old_max - range;

    // generate a point between [old_min, old_max - range] to avoid overflow
    set_range(old_min, new_max);
    Point pt = generate_a_point();

    // revert the range back
    set_range(old_min, old_max);

    return Query(pt.x, pt.x + range, pt.y, pt.y + range);
}

Point DataGenerator::generate_a_point() {
    return Point(mPtDist(mGenerator), mPtDist(mGenerator));
}

} // namespace ::Xiuge::RangeTree