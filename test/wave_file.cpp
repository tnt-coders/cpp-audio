#include "config.hpp"

#include <boost/type_index.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <tnt/audio/wave_file.hpp>
#include <tnt/dsp/multisignal.hpp>
#include <tnt/dsp/signal.hpp>
#include <tnt/dsp/signal_generator.hpp>
#include <tnt/math/comparison.hpp>

using namespace tnt;

TEMPLATE_TEST_CASE("wave_file construction", "[file][wave_file][constructor]", float, double)
{
    SECTION("pcm_uint8")
    {
        REQUIRE_NOTHROW(audio::wave_file<TestType>("data/wave_files/pcm_uint8.wav"));
    }

    SECTION("pcm_int16")
    {
        REQUIRE_NOTHROW(audio::wave_file<TestType>("data/wave_files/pcm_int16.wav"));
    }

    SECTION("pcm_int24")
    {
        REQUIRE_NOTHROW(audio::wave_file<TestType>("data/wave_files/pcm_int24.wav"));
    }

    SECTION("pcm_int32")
    {
        REQUIRE_NOTHROW(audio::wave_file<TestType>("data/wave_files/pcm_int32.wav"));
    }

    SECTION("ieee_float32")
    {
        REQUIRE_NOTHROW(audio::wave_file<TestType>("data/wave_files/ieee_float32.wav"));
    }

    SECTION("ieee_float64")
    {
        REQUIRE_NOTHROW(audio::wave_file<TestType>("data/wave_files/ieee_float64.wav"));
    }

    SECTION("file is empty")
    {
        REQUIRE_NOTHROW(audio::wave_file<TestType>("data/wave_files/empty.wav"));
    }

    SECTION("file doesn't exist")
    {
        REQUIRE_NOTHROW(audio::wave_file<TestType>("data/wave_files/nonexistent.wav"));
    }
}

TEMPLATE_TEST_CASE("wave_file accessors", "[file][wave_file][accessors]", float, double)
{
    const auto signal = signal_from_config<TestType>("data/wave_files/signal.dat");

    audio::wave_file<TestType> w("data/wave_files/ieee_float64.wav");

    CHECK(w.duration() == signal.duration());
    CHECK(w.sample_rate() == signal.sample_rate());
    CHECK(w.size() == signal.size());
    CHECK(w.channels() == signal.channels());
}

TEMPLATE_TEST_CASE("wave_file::read", "[file][wave_file][read]", float, double)
{
    const auto signal = signal_from_config<TestType>("data/wave_files/signal.dat");

    SECTION("pcm_uint8")
    {
        constexpr auto scale  = (static_cast<size_t>(std::numeric_limits<uint8_t>::max()) + 1) / 2;
        constexpr auto margin = static_cast<TestType>(1) / scale;

        audio::wave_file<TestType> w("data/wave_files/pcm_uint8.wav");

        CHECK(w.duration() == signal.duration());
        CHECK(w.sample_rate() == signal.sample_rate());
        CHECK(w.size() == signal.size());
        CHECK(w.channels() == signal.channels());

        const auto s = w.read();

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
        constexpr auto scale  = static_cast<size_t>(std::numeric_limits<int16_t>::max() + 1);
        constexpr auto margin = static_cast<TestType>(1) / scale;

        audio::wave_file<TestType> w("data/wave_files/pcm_int16.wav");

        CHECK(w.duration() == signal.duration());
        CHECK(w.sample_rate() == signal.sample_rate());
        CHECK(w.size() == signal.size());
        CHECK(w.channels() == signal.channels());

        const auto s = w.read();

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
        audio::wave_file<TestType> w("data/wave_files/pcm_int24.wav");

        CHECK(w.duration() == signal.duration());
        CHECK(w.sample_rate() == signal.sample_rate());
        CHECK(w.size() == signal.size());
        CHECK(w.channels() == signal.channels());

        const auto s = w.read();

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
        audio::wave_file<TestType> w("data/wave_files/pcm_int32.wav");

        CHECK(w.duration() == signal.duration());
        CHECK(w.sample_rate() == signal.sample_rate());
        CHECK(w.size() == signal.size());
        CHECK(w.channels() == signal.channels());

        const auto s = w.read();

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
        audio::wave_file<TestType> w("data/wave_files/ieee_float32.wav");

        CHECK(w.duration() == signal.duration());
        CHECK(w.sample_rate() == signal.sample_rate());
        CHECK(w.size() == signal.size());
        CHECK(w.channels() == signal.channels());

        const auto s = w.read();

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
        audio::wave_file<TestType> w("data/wave_files/ieee_float64.wav");

        CHECK(w.duration() == signal.duration());
        CHECK(w.sample_rate() == signal.sample_rate());
        CHECK(w.size() == signal.size());
        CHECK(w.channels() == signal.channels());

        const auto s = w.read();

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

    // TODO: more formats

    SECTION("file doesn't exist")
    {
        audio::wave_file<TestType> w("data/wave_files/nonexistent.wav");

        REQUIRE_THROWS_WITH(
            w.read(),
            Catch::Matchers::Equals(
                "Failed to open wave_file 'data/wave_files/nonexistent.wav' for reading"));
    }
}

TEMPLATE_TEST_CASE("wave_file::write", "[file][wave_file][write]", float, double)
{
    const auto signal = signal_from_config<TestType>("data/wave_files/signal.dat");

    // Need to use a different file name for each type so tests can run in parallel without conflict
    const auto test_type = boost::typeindex::type_id<TestType>().pretty_name();
    const auto file      = "data/wave_files/tmp-" + test_type + ".wav";

    SECTION("pcm_uint8")
    {
        constexpr auto scale  = (static_cast<size_t>(std::numeric_limits<uint8_t>::max()) + 1) / 2;
        constexpr auto margin = static_cast<TestType>(1) / scale;

        audio::wave_file<TestType> w(file);
        w.write(signal, audio::wave_format::pcm, audio::wave_subformat::pcm_uint8);

        CHECK(w.duration() == signal.duration());
        CHECK(w.sample_rate() == signal.sample_rate());
        CHECK(w.size() == signal.size());
        CHECK(w.channels() == signal.channels());

        const auto s = w.read();

        std::filesystem::remove(file);

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
        constexpr auto scale  = static_cast<size_t>(std::numeric_limits<int16_t>::max()) + 1;
        constexpr auto margin = static_cast<TestType>(1) / scale;

        audio::wave_file<TestType> w(file);
        w.write(signal, audio::wave_format::pcm, audio::wave_subformat::pcm_int16);

        CHECK(w.duration() == signal.duration());
        CHECK(w.sample_rate() == signal.sample_rate());
        CHECK(w.size() == signal.size());
        CHECK(w.channels() == signal.channels());

        const auto s = w.read();

        std::filesystem::remove(file);

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
        audio::wave_file<TestType> w(file);
        w.write(signal, audio::wave_format::pcm, audio::wave_subformat::pcm_int24);

        CHECK(w.duration() == signal.duration());
        CHECK(w.sample_rate() == signal.sample_rate());
        CHECK(w.size() == signal.size());
        CHECK(w.channels() == signal.channels());

        const auto s = w.read();

        std::filesystem::remove(file);

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
        audio::wave_file<TestType> w(file);
        w.write(signal, audio::wave_format::pcm, audio::wave_subformat::pcm_int32);

        CHECK(w.duration() == signal.duration());
        CHECK(w.sample_rate() == signal.sample_rate());
        CHECK(w.size() == signal.size());
        CHECK(w.channels() == signal.channels());

        const auto s = w.read();

        std::filesystem::remove(file);

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
        audio::wave_file<TestType> w(file);
        w.write(signal, audio::wave_format::ieee_float, audio::wave_subformat::ieee_float32);

        CHECK(w.duration() == signal.duration());
        CHECK(w.sample_rate() == signal.sample_rate());
        CHECK(w.size() == signal.size());
        CHECK(w.channels() == signal.channels());

        const auto s = w.read();

        std::filesystem::remove(file);

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
        audio::wave_file<TestType> w(file);
        w.write(signal, audio::wave_format::ieee_float, audio::wave_subformat::ieee_float64);

        CHECK(w.duration() == signal.duration());
        CHECK(w.sample_rate() == signal.sample_rate());
        CHECK(w.size() == signal.size());
        CHECK(w.channels() == signal.channels());

        const auto s = w.read();

        std::filesystem::remove(file);

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
