//
// Created by Xiuge Chen on 5/22/20.
//

#include <iostream>
#include <vector>
#include <spdlog/spdlog.h>

#include "org_range_tree.h"
#include "utils.h"

namespace Xiuge::RangeTree {

namespace {

inline bool in_range(Point pt, Query query) {
    return query.x_lower <= pt.x && pt.x <= query.x_upper
           && query.y_lower <= pt.y && pt.y <= query.y_upper;
}

}

void OrgRangeTree::construct_tree(std::vector<Point>& points, bool isNaive) {
    spdlog::info("[OrgRangeTree] Start original range tree construction");

    // in-place sort ascendingly by x, break tie by id
    sort(points.begin(), points.end(),
         [](const Point& a, const Point& b) -> bool
         {
             return a.x == b.x ? a.id < b.id : a.x < b.x;
         });

    // build on first dimension
    root = build_tree(points, 0, static_cast<int>(points.size() - 1), 1);

    // Uncomment if debug
    // spdlog::debug("[OrgRangeTree] Constructed tree in first dimension");
    // print_tree(root.get(), 0);

    // build on second dimension, either naively using O(n log^2 n) time, or smartly use O(n log n) time.
    if (isNaive) {
        spdlog::info("[OrgRangeTree] Start naive secondary tree construction");
        build_sec_dim_naive(root.get());
    }
    else {
        // in-place sort ascendingly by y, break tie by id
        sort(points.begin(), points.end(),
             [](const Point& a, const Point& b) -> bool
             {
                 return a.y == b.y ? a.id < b.id : a.y < b.y;
             });

        spdlog::info("[OrgRangeTree] Start smart secondary tree construction");
        build_sec_dim_smart(points, root.get());
    }
}

void OrgRangeTree::build_sec_dim_naive(OrgRangeTreeNode* node) {
    if (node == nullptr)
        return;

    if (unlikely(node->nextDimRoot))
        throw std::runtime_error("[DataGenerator] tree of next dimension already being created");

    std::vector<Point> points;
    in_order_traverse(node, points);

    // in-place sort ascendingly by y, break tie by id
    sort(points.begin(), points.end(),
         [](const Point& a, const Point& b) -> bool
         {
             return a.y == b.y ? a.id < b.id : a.y < b.y;
         });

    node->nextDimRoot = build_tree(points, 0, static_cast<int>(points.size() - 1), 2);

    // Uncomment if debug
    // spdlog::debug("[OrgRangeTree] Constructed secondary tree rooted at node x={}, y={}, id={}", node->point.x, node->point.y, node->point.id);
    // print_tree(node->nextDimRoot.get(), 0);

    // recursively build for all children
    build_sec_dim_naive(node->left.get());
    build_sec_dim_naive(node->right.get());
}

void OrgRangeTree::build_sec_dim_smart(std::vector<Point>& points, OrgRangeTreeNode* node) {
    if (node == nullptr)
        return;

    if (unlikely(node->nextDimRoot))
        throw std::runtime_error("[DataGenerator] tree of next dimension already being created");

    // create secondary tree
    node->nextDimRoot = build_tree(points, 0, static_cast<int>(points.size() - 1), 2);

    // Uncomment if debug
    // spdlog::debug("[OrgRangeTree] Constructed secondary tree rooted at node x={}, y={}, id={}", node->point.x, node->point.y, node->point.id);
    // print_tree(node->nextDimRoot.get(), 0);

    // partition points into left/right subtree (fist dimension) of node
    std::vector<Point> leftPts, rightPts;

    for (auto& point : points) {
        // break tie by id
        if (point.x == node->point.x) {
            if (point.id < node->point.id)
                leftPts.emplace_back(point);
            else if (point.id > node->point.id)
                rightPts.emplace_back(point);
            else
                continue;
        }
        else if(point.x < node->point.x)
            leftPts.emplace_back(point);
        else
            rightPts.emplace_back(point);
    }

    build_sec_dim_smart(leftPts, node->left.get());
    build_sec_dim_smart(rightPts, node->right.get());
}

std::unique_ptr<OrgRangeTreeNode> OrgRangeTree::build_tree(std::vector<Point>& points, const int start,
                                                         const int end, const int dim) {
    if (start > end)
        return nullptr;

    int mid = (start + end) / 2; // integer division

    std::unique_ptr<OrgRangeTreeNode> node(new OrgRangeTreeNode(points[static_cast<unsigned long>(mid)], dim));

    // construct tree in only first dimension
    node->left = build_tree(points, start, mid - 1, dim);
    node->right = build_tree(points, mid + 1, end, dim);

    // assign parent to each children
    if (node->left)
        node->left->parent = node.get();

    if (node->right)
        node->right->parent = node.get();

    return node;
}

void OrgRangeTree::report_points(Query query, std::vector<Point>& foundPts) {
    query_tree(root.get(), foundPts, query, true);
}

void OrgRangeTree::query_tree(OrgRangeTreeNode* node, std::vector<Point>& points, Query query, bool fstDim) {
    if (node == nullptr)
        return;

    // find the successor of x_min/y_min and the predecessor of x_max/y_max
    OrgRangeTreeNode* succ_min = tree_search(node, fstDim ? query.x_lower : query.y_lower, true, fstDim);
    OrgRangeTreeNode* pred_max = tree_search(node, fstDim ? query.x_upper : query.y_upper, false, fstDim);
    OrgRangeTreeNode* tree_iter = nullptr;

    // none of points are in range
    if (succ_min == nullptr || pred_max == nullptr)
        return;

    if (fstDim && succ_min->point.x > pred_max->point.x)
        return;
    if (!fstDim && succ_min->point.y > pred_max->point.y)
        return;

    // find the lowest common ancestor of succ_x_min and pred_x_max
    OrgRangeTreeNode* lca = find_lca(node, succ_min, pred_max, fstDim);

    // return lca if it is in range
    if (in_range(lca->point, query))
        points.emplace_back(lca->point);

    // For each node u other than lca on the path from lca to succ_min, add it if it is in range.
    // If first dimention and succ_min.x <= u.x, then report all the points in u’s right sub-tree whose y-coordinates
    // are in [y_lower, y_upper] in the secondary tree;
    // If second dimention and succ_min.y <= u.y, then report all the points in u’s right sub-tree;
    tree_iter = succ_min;
    while(tree_iter->point.id != lca->point.id) {
        if (in_range(tree_iter->point, query))
            points.emplace_back(tree_iter->point);

        if (fstDim) {
            if (succ_min->point.x <= tree_iter->point.x && tree_iter->right)
                query_tree(tree_iter->right->nextDimRoot.get(), points, query, false);
        }
        else {
            if (succ_min->point.y <= tree_iter->point.y)
                in_order_traverse(tree_iter->right.get(), points);
        }

        tree_iter = tree_iter->parent;
    }

    // for each node u other than lca on the path from lca to pred_max, add it if it is in range
    // If first dimention and pred_max.x >= u.x, then report all the points in u’s left sub-tree whose y-coordinates
    // are in [y_lower, y_upper] in the secondary tree;
    // If second dimention and pred_max.y >= u.y, then report all the points in u’s left sub-tree;
    tree_iter = pred_max;
    while(tree_iter->point.id != lca->point.id) {
        if (in_range(tree_iter->point, query))
            points.emplace_back(tree_iter->point);

        if (fstDim) {
            if (pred_max->point.x >= tree_iter->point.x && tree_iter->left)
                query_tree(tree_iter->left->nextDimRoot.get(), points, query, false);
        }
        else {
            if (pred_max->point.y >= tree_iter->point.y)
                in_order_traverse(tree_iter->left.get(), points);
        }

        tree_iter = tree_iter->parent;
    }
}

OrgRangeTreeNode* OrgRangeTree::tree_search(OrgRangeTreeNode* node, uint32_t value, bool findSucc, bool fstDim) {
    OrgRangeTreeNode* result = nullptr;

    if (findSucc) {
        while (node != nullptr) {
            if (fstDim) {
                if (node->point.x >= value) {
                    result = node;
                    node = node->left.get();
                }
                else
                    node = node->right.get();
            }
            else {
                if (node->point.y >= value) {
                    result = node;
                    node = node->left.get();
                }
                else
                    node = node->right.get();
            }
        }
    }
    else {
        while (node != nullptr) {
            if (fstDim) {
                if (node->point.x <= value) {
                    result = node;
                    node = node->right.get();
                }
                else
                    node = node->left.get();
            }
            else {
                if (node->point.y <= value) {
                    result = node;
                    node = node->right.get();
                }
                else
                    node = node->left.get();
            }
        }
    }

    return result;
}

OrgRangeTreeNode* OrgRangeTree::find_lca(OrgRangeTreeNode* node, OrgRangeTreeNode* succ, OrgRangeTreeNode* pred, bool fstDim) {
    OrgRangeTreeNode* tree_iter = node;

    while (tree_iter != nullptr) {
        if (tree_iter->point.id == succ->point.id || tree_iter->point.id == pred->point.id)
            return tree_iter;

        if (fstDim) {
            if (tree_iter->point.x >= succ->point.x) {
                if (tree_iter->point.x <= pred->point.x)
                    return tree_iter;
                else
                    tree_iter = tree_iter->left.get();
            }
            else {
                tree_iter = tree_iter->right.get();
            }
        }
        else {
            if (tree_iter->point.y >= succ->point.y) {
                if (tree_iter->point.y <= pred->point.y)
                    return tree_iter;
                else
                    tree_iter = tree_iter->left.get();
            }
            else {
                tree_iter = tree_iter->right.get();
            }
        }
    }

    return nullptr;
}

void OrgRangeTree::in_order_traverse(OrgRangeTreeNode* node, std::vector<Point>& points) {
    if (node) {
        in_order_traverse(node->left.get(), points);
        points.emplace_back(node->point);
        in_order_traverse(node->right.get(), points);
    }
}

void OrgRangeTree::print_tree(OrgRangeTreeNode* node, const int level) {
    if (node) {
        print_tree(node->right.get(), level + 1);

        for (int i = 0; i < level; i++)
            std::cout << "        ";

        std::cout << "(Id={" << node->point.id << "}, X={" << node->point.x << "}, Y={" <<
                    node->point.y << "}, parentId={" << (node->parent ? node->parent->point.id : 0) << "})" << std::endl;

        print_tree(node->left.get(), level + 1);
    }
}

} // namespace ::Xiuge::RangeTree