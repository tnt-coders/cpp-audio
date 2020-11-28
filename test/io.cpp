#include "approx.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <tnt/audio/io.hpp>

using namespace tnt;

TEMPLATE_TEST_CASE("file", "[file]", double, float)
{
    //const auto file = audio::file<TestType>("sinewave_64bit_100Hz_1000Hz-sample-rate_10-size_1-channels.wav");
    //const auto file = audio::file<TestType>("untitled.wav");
    //const auto signal = file->read();
}

//#include "approx.hpp"
//
//#include <tnt/audio/io.hpp>
//#include <tnt/audio/multisignal.hpp>
//#include <tnt/dsp/signal_generator.hpp>
//
//using namespace tnt;
//
//TEMPLATE_TEST_CASE("read", "[read]", double, float)
//{
//    const auto g = dsp::signal_generator<TestType>{ 1000, 10 };
//    const auto sine = g.sine(100);
//
//    SECTION("Read a .wav file")
//    {
//        const auto signal = audio::read<TestType>("sinewave_64bit_100Hz_1000Hz-sample-rate_10-size_1-channels.wav");
//
//        REQUIRE(signal.size() == sine.size());
//        REQUIRE(signal.channels() == 1);
//
//        for (size_t n = 0; n < signal.size(); ++n)
//        {
//            CHECK(signal[n][0] == approx(sine[n]));
//        }
//    }
//}