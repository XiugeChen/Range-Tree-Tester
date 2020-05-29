//
// Created by Xiuge Chen on 5/23/20.
//

#include <spdlog/spdlog.h>
#include <iostream>

#include "fc_range_tree.h"

namespace Xiuge::RangeTree {

namespace {

inline bool in_range(Point pt, Query query) {
    return query.x_lower <= pt.x && pt.x <= query.x_upper
           && query.y_lower <= pt.y && pt.y <= query.y_upper;
}

}

void FcRangeTree::construct_tree(std::vector<Point>& points, bool ) {
    spdlog::info("[FcRangeTree] Start factional-cascading range tree construction");

    // in-place sort ascendingly by x, and then by y, break tie by id
    sort(points.begin(), points.end());

    // build on first dimension
    mRoot = build_tree(points, 0, static_cast<int>(points.size() - 1));

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
        mRoot->secFCNodes.emplace_back(FcNode(point));

    build_sec_dim_array(mRoot.get());
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
    if (node == nullptr || node->secFCNodes.empty())
        return;

    int succ_left = 0, succ_right = 0;

    for (auto& secNode : node->secFCNodes) {
        secNode.successor_left = succ_left;
        secNode.successor_right = succ_right;

        if (node->left) {
            if (secNode.point < node->point) {
                node->left->secFCNodes.emplace_back(FcNode(secNode.point));
                ++succ_left;
            }
        }

        if (node->right) {
            if (secNode.point > node->point) {
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

void FcRangeTree::report_points(Query query, std::vector<Point>& foundPts) {
    FcRangeTreeNode* node = mRoot.get();

    // find the successor of x_min and the predecessor of x_max
    FcRangeTreeNode* succ_min = tree_search(node, query.x_lower, true);
    FcRangeTreeNode* pred_max = tree_search(node, query.x_upper, false);

    // none of points are in range
    if (succ_min == nullptr || pred_max == nullptr || succ_min->point.x > pred_max->point.x)
        return;

    // find the lowest common ancestor of succ_min and pred_max
    FcRangeTreeNode* lca = find_lca(node, succ_min, pred_max);

    // return lca if it is in range
    if (in_range(lca->point, query))
        foundPts.emplace_back(lca->point);

    // find the successor of y_min
    int index_succ_y_min = vector_search(lca->secFCNodes, query.y_lower, true);

    if (index_succ_y_min < 0)
        return;

    FcRangeTreeNode* tree_iter = nullptr;

    // For each node u other than lca on the path from lca to succ_min, add it if it is in range.
    // If first dimention and succ_min.x <= u.x, then report all the points in u’s right sub-tree whose y-coordinates
    // are in [y_lower, y_upper] in the secondary tree;
    if (lca->point.id != succ_min->point.id) {
        tree_iter = lca->left.get();
        int start_index = lca->secFCNodes[static_cast<unsigned long>(index_succ_y_min)].successor_left;
        FcNode start_node = tree_iter->secFCNodes[static_cast<unsigned long>(start_index)];

        while(true) {
            if (in_range(tree_iter->point, query))
                foundPts.emplace_back(tree_iter->point);

            if (succ_min->point.x <= tree_iter->point.x && tree_iter->right) {
                auto i = static_cast<unsigned long>(start_node.successor_right);
                auto size = tree_iter->right->secFCNodes.size();

                while(i < size && tree_iter->right->secFCNodes[i].point.y <= query.y_upper) {
                    foundPts.emplace_back(tree_iter->right->secFCNodes[i].point);
                    ++i;
                }
            }

            if (succ_min->point == tree_iter->point)
                break;
            else if (succ_min->point < tree_iter->point) {
                start_index = start_node.successor_left;
                tree_iter = tree_iter->left.get();
            }
            else {
                start_index = start_node.successor_right;
                tree_iter = tree_iter->right.get();
            }

            if (static_cast<unsigned long>(start_index) < tree_iter->secFCNodes.size())
                start_node = tree_iter->secFCNodes[static_cast<unsigned long>(start_index)];
            else
                break;
        }
    }

    // for each node u other than lca on the path from lca to pred_max, add it if it is in range
    // If first dimention and pred_max.x >= u.x, then report all the points in u’s left sub-tree whose y-coordinates
    // are in [y_lower, y_upper] in the secondary tree;
    if (lca->point.id != pred_max->point.id) {
        tree_iter = lca->right.get();
        int start_index = lca->secFCNodes[static_cast<unsigned long>(index_succ_y_min)].successor_right;
        FcNode start_node = tree_iter->secFCNodes[static_cast<unsigned long>(start_index)];

        while (true) {
            if (in_range(tree_iter->point, query))
                foundPts.emplace_back(tree_iter->point);

            if (pred_max->point.x >= tree_iter->point.x && tree_iter->left) {
                auto i = static_cast<unsigned long>(start_node.successor_left);
                auto size = tree_iter->left->secFCNodes.size();

                while (i < size  && tree_iter->left->secFCNodes[i].point.y <= query.y_upper) {
                    foundPts.emplace_back(tree_iter->left->secFCNodes[i].point);
                    ++i;
                }
            }

            if (pred_max->point == tree_iter->point)
                break;
            else if (pred_max->point < tree_iter->point) {
                start_index = start_node.successor_left;
                tree_iter = tree_iter->left.get();
            } else {
                start_index = start_node.successor_right;
                tree_iter = tree_iter->right.get();
            }

            if (static_cast<unsigned long>(start_index) < tree_iter->secFCNodes.size())
                start_node = tree_iter->secFCNodes[static_cast<unsigned long>(start_index)];
            else
                break;
        }
    }
}

FcRangeTreeNode* FcRangeTree::tree_search(FcRangeTreeNode* node, uint32_t value, bool findSucc) {
    FcRangeTreeNode* result = nullptr;

    if (findSucc) {
        while (node != nullptr) {
            if (node->point.x >= value) {
                result = node;
                node = node->left.get();
            }
            else
                node = node->right.get();
        }
    }
    else {
        while (node != nullptr) {
            if (node->point.x <= value) {
                result = node;
                node = node->right.get();
            }
            else
                node = node->left.get();
        }
    }

    return result;
}

int FcRangeTree::vector_search(std::vector<FcNode>& vector, uint32_t value, bool findSucc) {
    int result = -1;
    int upper = static_cast<int>(vector.size() - 1), lower = 0;

    if (findSucc) {
        while (upper > lower) {
            int mid = (upper - lower) / 2 + lower;

            if (vector[static_cast<unsigned long>(mid)].point.y >= value) {
                result = mid;
                upper = mid - 1;
            }
            else
                lower = mid + 1;
        }
    }
    else {
        while (upper > lower) {
            int mid = (upper - lower) / 2 + lower;

            if (vector[static_cast<unsigned long>(mid)].point.x <= value) {
                result = mid;
                lower = mid + 1;
            }
            else
                upper = mid - 1;
        }
    }

    return result;
}

FcRangeTreeNode* FcRangeTree::find_lca(FcRangeTreeNode* node, FcRangeTreeNode* succ, FcRangeTreeNode* pred) {
    FcRangeTreeNode* tree_iter = node;

    while (tree_iter != nullptr) {
        if (tree_iter->point == succ->point || tree_iter->point == pred->point)
            return tree_iter;

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

    return nullptr;
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