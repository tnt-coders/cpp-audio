#include "config.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <tnt/audio/file.hpp>
#include <tnt/math/comparison.hpp>

using namespace tnt;

TEMPLATE_TEST_CASE("file", "[file]", double, float)
{
    const auto signal = signal_from_config<TestType>("data/wave_files/signal.dat");

    SECTION("wave_file")
    {
        SECTION("read")
        {
            SECTION("pcm_uint8")
            {
                constexpr auto scale = (static_cast<size_t>(std::numeric_limits<uint8_t>::max())
                                        + 1)
                                     / 2;

                constexpr auto margin = static_cast<TestType>(1) / scale;

                const auto file = audio::file<TestType>("data/wave_files/pcm_uint8.wav");

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
                        CHECK_THAT(s[n][c], Catch::Matchers::WithinAbs(signal[n][c], margin));
                    }
                }
            }

            SECTION("pcm_int16")
            {
                constexpr auto scale = static_cast<size_t>(std::numeric_limits<int16_t>::max()) + 1;

                constexpr auto margin = static_cast<TestType>(1) / scale;

                const auto file = audio::file<TestType>("data/wave_files/pcm_int16.wav");

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
                        CHECK_THAT(s[n][c], Catch::Matchers::WithinAbs(signal[n][c], margin));
                    }
                }
            }

            SECTION("pcm_int24")
            {
                const auto file = audio::file<TestType>("data/wave_files/pcm_int24.wav");

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
                        CHECK(math::near(s[n][c], signal[n][c]));
                    }
                }
            }

            SECTION("pcm_int32")
            {
                const auto file = audio::file<TestType>("data/wave_files/pcm_int32.wav");

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
                        CHECK(math::near(s[n][c], signal[n][c]));
                    }
                }
            }

            SECTION("ieee_float32")
            {
                const auto file = audio::file<TestType>("data/wave_files/ieee_float32.wav");

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
                        CHECK(math::near(s[n][c], signal[n][c]));
                    }
                }
            }

            SECTION("ieee_float64")
            {
                const auto file = audio::file<TestType>("data/wave_files/ieee_float64.wav");

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
                        CHECK(math::near(s[n][c], signal[n][c]));
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
                    CHECK(math::near(s[n][c], signal[n][c]));
                }
            }
        }
    }
}
