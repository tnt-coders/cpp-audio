#include "constants.hpp"
#include <gtest/gtest.h>
#include <tnt/audio/wav_file.hpp>

using namespace tnt::audio;

template <typename T>
class WavFileTest : public ::testing::Test
{
protected:
    void WavFile_Constructor() const
    {

    }
};

TEST(WavFileTest, Test)
{
    const auto wavFile = WavFile<float>("sinewave_100Hz.wav");
    EXPECT_EQ(wavFile.GetChannels(), 1);
    EXPECT_EQ(wavFile.GetSampleRate(), 1000);

    const auto signal = wavFile.Read();
    ASSERT_EQ(signal.channels(), 1);
    ASSERT_EQ(signal.size(), 10);
    EXPECT_NEAR(signal[0][0], 0, constants::EPSILON);
    EXPECT_NEAR(signal[0][1], 0.588, constants::EPSILON);
    EXPECT_NEAR(signal[0][2], 0.951, constants::EPSILON);
    EXPECT_NEAR(signal[0][3], 0.951, constants::EPSILON);
    EXPECT_NEAR(signal[0][4], 0.588, constants::EPSILON);
    EXPECT_NEAR(signal[0][5], 0, constants::EPSILON);
    EXPECT_NEAR(signal[0][6], -0.588, constants::EPSILON);
    EXPECT_NEAR(signal[0][7], -0.951, constants::EPSILON);
    EXPECT_NEAR(signal[0][8], -0.951, constants::EPSILON);
    EXPECT_NEAR(signal[0][9], -0.588, constants::EPSILON);
}