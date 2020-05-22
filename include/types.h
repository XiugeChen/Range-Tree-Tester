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

struct BinaryTreeNode {
    BinaryTreeNode(Point newNode) {
        node = newNode;
    }

    Point node;

    std::unique_ptr<BinaryTreeNode> left{ nullptr };
    std::unique_ptr<BinaryTreeNode> right{ nullptr };
    BinaryTreeNode* parent{ nullptr };
};

// fractional cascading node
struct FCNode {
    FCNode(Point newNode) {
        node = newNode;
    }

    Point node;

    uint32_t successor = 0;
};

struct OrgRangeTreeNode {
    OrgRangeTreeNode(Point node) {
        treeNode.reset(new BinaryTreeNode(node));
    }

    std::unique_ptr<BinaryTreeNode> treeNode{ nullptr };
    std::unique_ptr<BinaryTreeNode> secTreeNode{ nullptr };
};

struct FcRangeTreeNode {
    FcRangeTreeNode(Point node) {
        treeNode.reset(new BinaryTreeNode(node));
    }

    std::unique_ptr<BinaryTreeNode> treeNode{ nullptr };
    std::vector<FCNode> secFCNode;
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
