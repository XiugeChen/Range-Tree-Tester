#include <spdlog/spdlog.h>

#include "experiment_app.h"

using namespace ::Xiuge::RangeTree;

int main() {
    spdlog::set_level(spdlog::level::info);
    spdlog::info("Start range tree tester");

    ExperimentApp experiment;
    auto data_len_base = static_cast<const uint32_t>(2 * pow(10, 3));

    // test with construction time, vary data length
    std::vector<uint32_t> dataLens{data_len_base, 2 * data_len_base, 4 * data_len_base, 8 * data_len_base,
                                   16 * data_len_base, 32 * data_len_base, 64 * data_len_base, 128 * data_len_base,
                                   256 * data_len_base, 512 * data_len_base};

    experiment.construct_time_data_length(dataLens);
    /*
    // test with query time, vary data length
    std::vector<uint32_t> queryDataLens{data_len_base, 2 * data_len_base, 4 * data_len_base, 8 * data_len_base,
                                   16 * data_len_base, 32 * data_len_base, 64 * data_len_base, 128 * data_len_base,
                                   256 * data_len_base, 512 * data_len_base};

    experiment.query_time_data_length(queryDataLens);

    // test with query time, vary query range
    std::vector<double> queryRanges{0.01, 0.02, 0.05, 0.1, 0.2};

    experiment.query_time_query_range(queryRanges);
    */
    return 0;
}
