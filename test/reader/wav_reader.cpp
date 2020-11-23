#include "../approx.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <tnt/audio/multisignal.hpp>
#include <tnt/audio/reader/wav_reader.hpp>
#include <tnt/dsp/signal_generator.hpp>

using namespace tnt;

TEMPLATE_TEST_CASE("wav_reader construction", "[reader][wav_reader][construction]", double, float)
{
    audio::wav_reader<TestType> r("sinewave_64bit_100Hz_1000Hz-sample-rate_10-size_1-channels.wav");
    CHECK(r.sample_rate() == 1000);
    CHECK(r.size() == 10);
    CHECK(r.channels() == 1);
}

TEMPLATE_TEST_CASE("wav_reader accessors", "[reader][wav_reader][accessors]", double, float)
{
    audio::wav_reader<TestType> r("sinewave_64bit_100Hz_1000Hz-sample-rate_10-size_1-channels.wav");

    SECTION("sample_rate")
    {
        CHECK(r.sample_rate() == 1000);
    }

    SECTION("size")
    {
        CHECK(r.size() == 10);
    }

    SECTION("channels")
    {
        CHECK(r.channels() == 1);
    }
}

TEMPLATE_TEST_CASE("wav_reader::read", "[reader][wav_reader][read]", double, float)
{
    audio::wav_reader<TestType> r("sinewave_64bit_100Hz_1000Hz-sample-rate_10-size_1-channels.wav");
    dsp::signal_generator<TestType> g{ r.sample_rate(), r.size() };
    const auto sine = g.sine(100);
    auto buffer = r.buffer(r.size() / 2);

    size_t offset = 0;
    while (!r.eof())
    {
        r.read(&buffer);
        for (size_t n = 0; n < buffer.size(); ++n)
        {
            if (n + offset < sine.size())
            {
                CHECK(buffer[n][0] == approx(sine[n + offset]));
            }
            else
            {
                CHECK(buffer[n][0] == approx(0));
            }
        }

        offset += buffer.size();
    }
}

//#include "constants.hpp"
//#include <gtest/gtest.h>
//#include <tnt/audio/wav_reader.hpp>
//
//using namespace tnt;
//
//template <typename T>
//class WavFileTest : public ::testing::Test
//{
//protected:
//    void WavFile_Constructor() const
//    {
//        const auto wavFile = audio::wav_reader<T>("sinewave_1channel_64bit_100Hz.wav");
//        EXPECT_EQ(wavFile.GetChannels(), 1);
//        EXPECT_EQ(wavFile.GetSampleRate(), 1000);
//    }
//};
//
//TEST(WavFileTest, Test)
//{
//    const auto wavFile = audio::wav_reader<float>("sinewave_1channel_64bit_100Hz.wav");
//    const auto signal = wavFile.Read();
//    ASSERT_EQ(signal.channels(), 1);
//    ASSERT_EQ(signal.size(), 10);
//    EXPECT_NEAR(signal[0][0], 0, constants::EPSILON);
//    EXPECT_NEAR(signal[0][1], 0.588, constants::EPSILON);
//    EXPECT_NEAR(signal[0][2], 0.951, constants::EPSILON);
//    EXPECT_NEAR(signal[0][3], 0.951, constants::EPSILON);
//    EXPECT_NEAR(signal[0][4], 0.588, constants::EPSILON);
//    EXPECT_NEAR(signal[0][5], 0, constants::EPSILON);
//    EXPECT_NEAR(signal[0][6], -0.588, constants::EPSILON);
//    EXPECT_NEAR(signal[0][7], -0.951, constants::EPSILON);
//    EXPECT_NEAR(signal[0][8], -0.951, constants::EPSILON);
//    EXPECT_NEAR(signal[0][9], -0.588, constants::EPSILON);
//}