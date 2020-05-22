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



private:
    std::unique_ptr<BinaryTreeNode> root{nullptr};
};

} // namespace ::Xiuge::RangeTree

#endif //RANGETREE_ORG_RANGE_TREE_H
