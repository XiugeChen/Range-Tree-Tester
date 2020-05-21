//
// Created by Xiuge Chen on 5/21/20.
//

#ifndef RANGETREE_TYPES_H
#define RANGETREE_TYPES_H

#include <cstdint>

namespace Xiuge::RangeTree {

// A point in two dimensional space
struct Point {
    Point(uint32_t new_x, uint32_t new_y) {
        x = new_x;
        y = new_y;
    }

    uint32_t id = 0;
    uint32_t x = 0;
    uint32_t y = 0;
};

struct Query {
    Query(uint32_t new_x_lower, uint32_t new_x_upper, uint32_t new_y_lower, uint32_t new_y_upper) {
        x_lower = new_x_lower;
        x_upper = new_x_upper;
        y_lower = new_y_lower;
        y_upper = new_y_upper;
    }

    uint32_t x_lower = 0;
    uint32_t x_upper = 0;

    uint32_t y_lower = 0;
    uint32_t y_upper = 0;
};

} // namespace ::Xiuge::RangeTree

#endif //RANGETREE_TYPES_H
