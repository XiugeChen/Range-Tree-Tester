//
// Created by Xiuge Chen on 5/23/20.
//

#ifndef RANGETREE_FD_RANGE_TREE_H
#define RANGETREE_FD_RANGE_TREE_H

#include "types.h"

namespace Xiuge::RangeTree {

/**
 * Implementation of Fractional Cascading Range Tree
 */
class FcRangeTree : IRangeTree {
public:
    void construct_tree(std::vector<Point>& points, bool ) override;

    std::vector<Point> report_points(Query query) override;

private:
    /* construction helper function */
    /**
     * Build a weighted balance binary search tree based on a sorted vector of points in O(n) time
     * @param points A vector of points, must be sorted ascedingly.
     * @param start Start point of the vector.
     * @param end End point of the vector.
     * @return A pointer point to the root of the tree.
     */
    static std::unique_ptr<FcRangeTreeNode> build_tree(std::vector<Point>& points, const int start, const int end);

    /**
     * Recursively build secondary fractional cascading array for each of the node in the range tree
     * @param node Start node
     */
    void build_sec_dim_array(FcRangeTreeNode* node);

    /* tree traverse function */
    /**
     * Print the tree to stdout, mainly used for debug purpose
     * @param node
     * @param level
     */
    static void print_tree(FcRangeTreeNode* node, const int level);

    std::unique_ptr<FcRangeTreeNode> root{nullptr};
};

} // namespace ::Xiuge::RangeTree

#endif //RANGETREE_FD_RANGE_TREE_H
