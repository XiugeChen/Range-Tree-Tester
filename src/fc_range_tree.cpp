//
// Created by Xiuge Chen on 5/23/20.
//

#include <spdlog/spdlog.h>
#include <iostream>

#include "fc_range_tree.h"

namespace Xiuge::RangeTree {

void FcRangeTree::construct_tree(std::vector<Point>& points, bool ) {
    spdlog::info("[FcRangeTree] Start factional-cascading range tree construction");

    // in-place sort ascendingly by x, break tie by id
    sort(points.begin(), points.end(),
         [](const Point& a, const Point& b) -> bool
         {
             return a.x == b.x ? a.id < b.id : a.x < b.x;
         });

    // build on first dimension
    root = build_tree(points, 0, static_cast<int>(points.size() - 1));

    // Uncomment if debug
    // spdlog::debug("[OrgRangeTree] Constructed tree in first dimension");
    // print_tree(root.get(), 0);

    // build second dimension array for each node
    spdlog::info("[FcRangeTree] Start secondary factional-cascading construction");

    // in-place sort ascendingly by y, break tie by id
    sort(points.begin(), points.end(),
         [](const Point& a, const Point& b) -> bool
         {
             return a.y == b.y ? a.id < b.id : a.y < b.y;
         });

    for (auto point : points)
        root->secFCNodes.emplace_back(FcNode(point));

    build_sec_dim_array(root.get());
}

std::unique_ptr<FcRangeTreeNode> FcRangeTree::build_tree(std::vector<Point>& points, const int start, const int end) {
    if (start > end)
        return nullptr;

    int mid = (start + end) / 2; // integer division

    std::unique_ptr<FcRangeTreeNode> node(new FcRangeTreeNode(points[static_cast<unsigned long>(mid)]));

    // construct tree in only first dimension
    node->left = build_tree(points, start, mid - 1);
    node->right = build_tree(points, mid + 1, end);

    // assign parent to each children
    if (node->left)
        node->left->parent = node.get();

    if (node->right)
        node->right->parent = node.get();

    return node;
}

void FcRangeTree::build_sec_dim_array(FcRangeTreeNode* node) {
    if (node == nullptr || node->secFCNodes.size() <= 0)
        return;

    int succ_left = 0, succ_right = 0;

    for (auto& secNode : node->secFCNodes) {
        if (secNode.point.id == node->point.id)
            continue;

        secNode.successor_left = succ_left;
        secNode.successor_right = succ_right;

        if (node->left) {
            if (secNode.point.x < node->point.x
               || (secNode.point.x == node->point.x && secNode.point.id < node->point.id)) {

                node->left->secFCNodes.emplace_back(FcNode(secNode.point));
                ++succ_left;
            }
        }

        if (node->right) {
            if (secNode.point.x > node->point.x
                || (secNode.point.x == node->point.x && secNode.point.id > node->point.id)) {

                node->right->secFCNodes.emplace_back(FcNode(secNode.point));
                ++succ_right;
            }
        }
    }

    // Uncomment for debug
    // for (auto secNode : node->secFCNodes) {
    //     std::cout << secNode.point.id << "-" << secNode.point.x << "-" << secNode.point.y
    //               << "{" << secNode.successor_left << "," << secNode.successor_right << "};  ";
    // }
    // std::cout << std::endl;

    build_sec_dim_array(node->left.get());
    build_sec_dim_array(node->right.get());
}

std::vector<Point> FcRangeTree::report_points(Query query) {

}

void FcRangeTree::print_tree(FcRangeTreeNode* node, const int level) {
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