#include <iostream>
#include <spdlog/spdlog.h>

#include "org_range_tree.h"
#include "data_generator.h"

using namespace ::Xiuge::RangeTree;

int main() {
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Start range tree tester");

    OrgRangeTree tree;
    DataGenerator dataGenerator;

    dataGenerator.set_range(1, 10);
    std::vector<Point> points = dataGenerator.generate_point_set(10);

    tree.construct_tree(points, false);

    for (int i = 8; i >= 0; i -= 2) {
        Query query = dataGenerator.generate_a_query(i);
        auto pts = tree.report_points(query);

        std::cout << "query with range: [" << query.x_lower << "," << query.x_upper << "] * [" << query.y_lower << "," << query.y_upper << "]\n";
        for (int j = 0; j < pts.size(); ++j) {
            std::cout << pts[j].id << "-" << pts[j].x << "-"  << pts[j].y  << ",  ";
        }
        std::cout << "\n";
    }

    return 0;
}
