//
// Created by Xiuge Chen on 5/21/20.
//

#ifndef RANGETREE_ORG_RANGE_TREE_H
#define RANGETREE_ORG_RANGE_TREE_H

#include <memory>

#include "types.h"

namespace Xiuge::RangeTree {

/**
 * Implementation of Range Tree
 */
class OrgRangeTree : IRangeTree {
public:
    void construct_tree(std::vector<Point>& points, bool isNaive) override;

    std::vector<Point> report_points(Query query) override;

private:
    /* construction helper function */
    /**
     * Naively and recursively build the secondary range tree for the given tree rooted at node, O(n log^2 n) time
     * @param node
     */
    static void build_sec_dim_naive(OrgRangeTreeNode* node);

    /**
     * Smartly and recursively build the secondary range tree for the given tree rooted at node, O(n log n) time
     * @param points All points that is in the subtree rooted at node, sorted ascendingly by y.
     * @param node
     */
    static void build_sec_dim_smart(std::vector<Point>& points, OrgRangeTreeNode* node);

    /**
     * Build a weighted balance binary search tree based on a sorted vector of points in O(n) time
     * @param points A vector of points, must be sorted ascedingly.
     * @param start Start point of the vector.
     * @param end End point of the vector.
     * @param dim The dimension that the tree is in
     * @return A pointer point to the root of the tree.
     */
    static std::unique_ptr<OrgRangeTreeNode> build_tree(std::vector<Point>& points, const int start, const int end, int dim);

    /* query helper function */
    /**
     * Search along a tree at either first dimension or second dimension, report all nodes that is in query range
     * @param node Start node, or the root
     * @param points Store all points that are in the query range
     * @param query
     * @param fstDim True if search along the first dimension
     */
    void search_tree(OrgRangeTreeNode* node, std::vector<Point>& points, Query query, bool fstDim);

    /**
     * Search among the tree, find either the successor or predecessor of the given value
     * @param node
     * @param value
     * @param findSucc True if return successor
     * @param fstDim True if search along the first dimension
     * @return The successor or predecessor of the given value
     */
    OrgRangeTreeNode* tree_search(OrgRangeTreeNode* node, uint32_t value, bool findSucc, bool fstDim);

    /**
     * Find the lowest common ancestor of given two tree node.
     * @param node Start root
     * @param succ
     * @param pred
     * @param fstDim True if search along the first dimension
     * @return The lowest common ancestor of given two tree node.
     */
    OrgRangeTreeNode* find_lca(OrgRangeTreeNode* node, OrgRangeTreeNode* succ, OrgRangeTreeNode* pred, bool fstDim);

    /* tree traverse function */
    /**
     * In order traverse the first dimension of a tree rooted at given node
     * @param points A vector storing the traverse results
     * @param node
     */
    static void in_order_traverse(OrgRangeTreeNode* node, std::vector<Point>& points);

    /**
     * Print the tree to stdout, mainly used for debug purpose
     * @param node
     * @param level
     */
    static void print_tree(OrgRangeTreeNode* node, const int level);

    std::unique_ptr<OrgRangeTreeNode> root{nullptr};
};

} // namespace ::Xiuge::RangeTree

#endif //RANGETREE_ORG_RANGE_TREE_H
