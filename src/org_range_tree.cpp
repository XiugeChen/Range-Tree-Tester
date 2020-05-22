//
// Created by Xiuge Chen on 5/22/20.
//

#include <iostream>
#include <vector>
#include <spdlog/spdlog.h>

#include "org_range_tree.h"
#include "utils.h"

namespace Xiuge::RangeTree {

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

std::vector<Point> OrgRangeTree::report_points(Query query) {
    // TODO:
}

void OrgRangeTree::build_sec_dim_naive(OrgRangeTreeNode* node) {
    if (node == nullptr)
        return;

    if (unlikely(node->nextDimRoot))
        throw std::runtime_error("[DataGenerator] tree of next dimension already being created");

    std::vector<Point> points;
    in_order_traverse(points, node);

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
                leftPts.push_back(point);
            else if (point.id > node->point.id)
                rightPts.push_back(point);
            else
                continue;
        }
        else if(point.x < node->point.x)
            leftPts.push_back(point);
        else
            rightPts.push_back(point);
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

void OrgRangeTree::in_order_traverse(std::vector<Point>& points, OrgRangeTreeNode* node) {
    if (node) {
        in_order_traverse(points, node->left.get());
        points.push_back(node->point);
        in_order_traverse(points, node->right.get());
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