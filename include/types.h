//
// Created by Xiuge Chen on 5/21/20.
//

#ifndef RANGETREE_TYPES_H
#define RANGETREE_TYPES_H

#include <cstdint>
#include <memory>
#include <vector>

namespace Xiuge::RangeTree {

// A point in two dimensional space
struct Point {
    Point(uint32_t new_x, uint32_t new_y) {
        id = 0;
        x = new_x;
        y = new_y;
    }

    Point() {
        id = 0;
        x = 0;
        y = 0;
    }

    uint32_t id;
    uint32_t x;
    uint32_t y;
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

// fractional cascading node
struct FcNode {
    FcNode(Point newPoint) {
        point = newPoint;
    }

    Point point;

    int32_t successor_left = -1;
    int32_t successor_right = -1;
};

struct OrgRangeTreeNode {
    OrgRangeTreeNode(Point newPoint, int newDimension) {
        point = newPoint;
        dimension = newDimension;
    }

    Point point;
    int dimension;

    std::unique_ptr<OrgRangeTreeNode> left{ nullptr };
    std::unique_ptr<OrgRangeTreeNode> right{ nullptr };

    std::unique_ptr<OrgRangeTreeNode> nextDimRoot{ nullptr };

    OrgRangeTreeNode* parent{ nullptr };
};

struct FcRangeTreeNode {
    FcRangeTreeNode(Point newPoint) {
        point = newPoint;
    }

    Point point;

    std::unique_ptr<FcRangeTreeNode> left{ nullptr };
    std::unique_ptr<FcRangeTreeNode> right{ nullptr };

    std::vector<FcNode> secFCNodes;

    FcRangeTreeNode* parent{ nullptr };
};

class IRangeTree {
public:
    /**
     * Construct a range tree based on the given points
     * @param points Points
     * @param isNaive Whether use the naive O(n log^2 n) approach or use more advanced O(n log n) approach
     */
    virtual void construct_tree(std::vector<Point>& points, bool isNaive) = 0;

    /**
     * Report all the points that is in the query range
     * @param query Query that specify the range in each dimension
     * @return All points that are in the query range.
     */
    virtual std::vector<Point> report_points(Query query) = 0;
};

} // namespace ::Xiuge::RangeTree

#endif //RANGETREE_TYPES_H
