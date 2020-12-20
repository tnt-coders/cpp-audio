#include "approx.hpp"
#include "config.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <tnt/audio/io.hpp>

using namespace tnt;

TEMPLATE_TEST_CASE("file", "[file]", double, float)
{
    const auto signal = signal_from_config<TestType>("data/wave_files/signal.dat");

    SECTION("WaveFile")
    {
        SECTION("read")
        {
            SECTION("PCM_U8")
            {
                constexpr auto scale = (static_cast<size_t>(std::numeric_limits<uint8_t>::max()) + 1) / 2;
                constexpr auto epsilon = static_cast<TestType>(1) / scale;

                const auto file = audio::file<TestType>("data/wave_files/PCM_U8.wav");

                CHECK(file->duration() == signal.duration());
                CHECK(file->sample_rate() == signal.sample_rate());
                CHECK(file->size() == signal.size());
                CHECK(file->channels() == signal.channels());

                const auto s = file->read();

                REQUIRE(s.size() == signal.size());
                REQUIRE(s.channels() == signal.channels());

                for (size_t n = 0; n < s.size(); ++n)
                {
                    for (size_t c = 0; c < s.channels(); ++c)
                    {
                        CHECK(s[n][c] == approx(signal[n][c]).epsilon(epsilon));
                    }
                }
            }

            SECTION("PCM_16")
            {
                constexpr auto scale = static_cast<size_t>(std::numeric_limits<int16_t>::max()) + 1;
                constexpr auto epsilon = static_cast<TestType>(1) / scale;

                const auto file = audio::file<TestType>("data/wave_files/PCM_16.wav");

                CHECK(file->duration() == signal.duration());
                CHECK(file->sample_rate() == signal.sample_rate());
                CHECK(file->size() == signal.size());
                CHECK(file->channels() == signal.channels());

                const auto s = file->read();

                REQUIRE(s.size() == signal.size());
                REQUIRE(s.channels() == signal.channels());

                for (size_t n = 0; n < s.size(); ++n)
                {
                    for (size_t c = 0; c < s.channels(); ++c)
                    {
                        CHECK(s[n][c] == approx(signal[n][c]).epsilon(epsilon));
                    }
                }
            }

            SECTION("PCM_24")
            {
                const auto file = audio::file<TestType>("data/wave_files/PCM_24.wav");

                CHECK(file->duration() == signal.duration());
                CHECK(file->sample_rate() == signal.sample_rate());
                CHECK(file->size() == signal.size());
                CHECK(file->channels() == signal.channels());

                const auto s = file->read();

                REQUIRE(s.size() == signal.size());
                REQUIRE(s.channels() == signal.channels());

                for (size_t n = 0; n < s.size(); ++n)
                {
                    for (size_t c = 0; c < s.channels(); ++c)
                    {
                        CHECK(s[n][c] == approx(signal[n][c]));
                    }
                }
            }

            SECTION("PCM_32")
            {
                const auto file = audio::file<TestType>("data/wave_files/PCM_32.wav");

                CHECK(file->duration() == signal.duration());
                CHECK(file->sample_rate() == signal.sample_rate());
                CHECK(file->size() == signal.size());
                CHECK(file->channels() == signal.channels());

                const auto s = file->read();

                REQUIRE(s.size() == signal.size());
                REQUIRE(s.channels() == signal.channels());

                for (size_t n = 0; n < s.size(); ++n)
                {
                    for (size_t c = 0; c < s.channels(); ++c)
                    {
                        CHECK(s[n][c] == approx(signal[n][c]));
                    }
                }
            }

            SECTION("FLOAT")
            {
                const auto file = audio::file<TestType>("data/wave_files/FLOAT.wav");

                CHECK(file->duration() == signal.duration());
                CHECK(file->sample_rate() == signal.sample_rate());
                CHECK(file->size() == signal.size());
                CHECK(file->channels() == signal.channels());

                const auto s = file->read();

                REQUIRE(s.size() == signal.size());
                REQUIRE(s.channels() == signal.channels());

                for (size_t n = 0; n < s.size(); ++n)
                {
                    for (size_t c = 0; c < s.channels(); ++c)
                    {
                        CHECK(s[n][c] == approx(signal[n][c]));
                    }
                }
            }

            SECTION("DOUBLE")
            {
                const auto file = audio::file<TestType>("data/wave_files/DOUBLE.wav");

                CHECK(file->duration() == signal.duration());
                CHECK(file->sample_rate() == signal.sample_rate());
                CHECK(file->size() == signal.size());
                CHECK(file->channels() == signal.channels());

                const auto s = file->read();

                REQUIRE(s.size() == signal.size());
                REQUIRE(s.channels() == signal.channels());

                for (size_t n = 0; n < s.size(); ++n)
                {
                    for (size_t c = 0; c < s.channels(); ++c)
                    {
                        CHECK(s[n][c] == approx(signal[n][c]));
                    }
                }
            }
        }

        SECTION("write")
        {
            const auto file = audio::file<TestType>("data/wave_files/tmp.wav");
            file->write(signal);

            CHECK(file->duration() == signal.duration());
            CHECK(file->sample_rate() == signal.sample_rate());
            CHECK(file->size() == signal.size());
            CHECK(file->channels() == signal.channels());

            const auto s = file->read();

            REQUIRE(s.size() == signal.size());
            REQUIRE(s.channels() == signal.channels());

            for (size_t n = 0; n < s.size(); ++n)
            {
                for (size_t c = 0; c < s.channels(); ++c)
                {
                    CHECK(s[n][c] == approx(signal[n][c]));
                }
            }
        }
    }
}
