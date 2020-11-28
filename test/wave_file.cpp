#include "approx.hpp"
#include "config.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <tnt/audio/wave_file.hpp>

using namespace tnt;

TEMPLATE_TEST_CASE("wave_file construction", "[file][wave_file][constructor]", float, double)
{
    const auto signal_data = read_config("data/wave_files/signal.dat");

    //TODO:
    //FAIL();
}