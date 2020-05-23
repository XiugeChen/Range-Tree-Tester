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

    void report_points(Query query, std::vector<Point>& foundPts) override;

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
    static void build_sec_dim_array(FcRangeTreeNode* node);

    /* range query helper function */
    /**
     * Search among the tree, find either the successor or predecessor of the given value
     * @param node
     * @param value
     * @param findSucc True if return successor
     * @return The successor or predecessor of the given value
     */
    static FcRangeTreeNode* tree_search(FcRangeTreeNode* node, uint32_t value, bool findSucc);

    /**
     * Search among the vector, find either the successor or predecessor of the given value
     * @param node
     * @param value
     * @param findSucc True if return successor
     * @return The successor or predecessor of the given value
     */
    int vector_search(std::vector<FcNode>& vector, uint32_t value, bool findSucc);

    /**
     * Find the lowest common ancestor of given two tree node.
     * @param node Start root
     * @param succ
     * @param pred
     * @return The lowest common ancestor of given two tree node.
     */
    static FcRangeTreeNode* find_lca(FcRangeTreeNode* node, FcRangeTreeNode* succ, FcRangeTreeNode* pred);

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
