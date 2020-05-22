#include <spdlog/spdlog.h>

#include "org_range_tree.h"
#include "data_generator.h"

using namespace ::Xiuge::RangeTree;

int main() {
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Start range tree tester");

    OrgRangeTree tree;
    DataGenerator dataGenerator;

    dataGenerator.set_range(1, 100);
    std::vector<Point> points = dataGenerator.generate_point_set(10);

    tree.construct_tree(points, false);

    return 0;
}
