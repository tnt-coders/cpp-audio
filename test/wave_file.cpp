#include "config.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>
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

TEMPLATE_TEST_CASE("WaveFile construction", "[file][WaveFile][constructor]", float, double)
{
    SECTION("PCM_U8")
    {
        REQUIRE_NOTHROW(audio::WaveFile<TestType>("data/wave_files/PCM_U8.wav"));
    }

    SECTION("PCM_16")
    {
        REQUIRE_NOTHROW(audio::WaveFile<TestType>("data/wave_files/PCM_16.wav"));
    }

    SECTION("PCM_24")
    {
        REQUIRE_NOTHROW(audio::WaveFile<TestType>("data/wave_files/PCM_24.wav"));
    }

    SECTION("PCM_32")
    {
        REQUIRE_NOTHROW(audio::WaveFile<TestType>("data/wave_files/PCM_32.wav"));
    }

    SECTION("FLOAT")
    {
        REQUIRE_NOTHROW(audio::WaveFile<TestType>("data/wave_files/FLOAT.wav"));
    }

    SECTION("DOUBLE")
    {
        REQUIRE_NOTHROW(audio::WaveFile<TestType>("data/wave_files/DOUBLE.wav"));
    }

    SECTION("File is empty")
    {
        REQUIRE_NOTHROW(audio::WaveFile<TestType>("data/wave_files/empty.wav"));
    }

    SECTION("File doesn't exist")
    {
        REQUIRE_NOTHROW(audio::WaveFile<TestType>("data/wave_files/nonexistent.wav"));
    }
}

TEMPLATE_TEST_CASE("WaveFile accessors", "[file][WaveFile][accessors]", float, double)
{
    const auto signal = signal_from_config<TestType>("data/wave_files/signal.dat");

    audio::WaveFile<TestType> w("data/wave_files/DOUBLE.wav");

    CHECK(w.duration() == signal.duration());
    CHECK(w.sample_rate() == signal.sample_rate());
    CHECK(w.size() == signal.size());
    CHECK(w.channels() == signal.channels());
}

TEMPLATE_TEST_CASE("WaveFile::read", "[file][WaveFile][read]", float, double)
{
    const auto signal = signal_from_config<TestType>("data/wave_files/signal.dat");

    SECTION("PCM_U8")
    {
        constexpr auto scale = (static_cast<size_t>(std::numeric_limits<uint8_t>::max()) + 1) / 2;

        constexpr auto margin = static_cast<TestType>(1) / scale;

        audio::WaveFile<TestType> w("data/wave_files/PCM_U8.wav");

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

    SECTION("PCM_16")
    {
        constexpr auto scale = static_cast<size_t>(std::numeric_limits<int16_t>::max() + 1);

        constexpr auto margin = static_cast<TestType>(1) / scale;

        audio::WaveFile<TestType> w("data/wave_files/PCM_16.wav");

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

    SECTION("PCM_24")
    {
        // No need to use "margin" anymore because 24 bit data is more accurate
        audio::WaveFile<TestType> w("data/wave_files/PCM_24.wav");

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

    SECTION("PCM_32")
    {
        audio::WaveFile<TestType> w("data/wave_files/PCM_32.wav");

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

    SECTION("FLOAT")
    {
        audio::WaveFile<TestType> w("data/wave_files/FLOAT.wav");

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

    SECTION("DOUBLE")
    {
        audio::WaveFile<TestType> w("data/wave_files/DOUBLE.wav");

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

    SECTION("File doesn't exist")
    {
        audio::WaveFile<TestType> w("data/wave_files/nonexistent.wav");

        REQUIRE_THROWS_WITH(
            w.read(),
            Catch::Matchers::Equals(
                "Failed to open WaveFile 'data/wave_files/nonexistent.wav' for reading"));
    }
}

TEMPLATE_TEST_CASE("WaveFile::write", "[file][WaveFile][write]", float, double)
{
    const auto signal = signal_from_config<TestType>("data/wave_files/signal.dat");

    SECTION("PCM_U8")
    {
        constexpr auto scale = (static_cast<size_t>(std::numeric_limits<uint8_t>::max()) + 1) / 2;

        constexpr auto margin = static_cast<TestType>(1) / scale;

        audio::WaveFile<TestType> w("data/wave_files/tmp.wav");
        w.write(signal, audio::WaveFormat::PCM, audio::WaveDataType::UINT8);

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

    SECTION("PCM_16")
    {
        constexpr auto scale = static_cast<size_t>(std::numeric_limits<int16_t>::max()) + 1;

        constexpr auto margin = static_cast<TestType>(1) / scale;

        audio::WaveFile<TestType> w("data/wave_files/tmp.wav");
        w.write(signal, audio::WaveFormat::PCM, audio::WaveDataType::INT16);

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

    SECTION("PCM_24")
    {
        audio::WaveFile<TestType> w("data/wave_files/tmp.wav");
        w.write(signal, audio::WaveFormat::PCM, audio::WaveDataType::INT24);

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

    SECTION("PCM_32")
    {
        audio::WaveFile<TestType> w("data/wave_files/tmp.wav");
        w.write(signal, audio::WaveFormat::PCM, audio::WaveDataType::INT32);

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

    SECTION("FLOAT")
    {
        audio::WaveFile<TestType> w("data/wave_files/tmp.wav");
        w.write(signal, audio::WaveFormat::IEEE_FLOAT, audio::WaveDataType::FLOAT);

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

    SECTION("DOUBLE")
    {
        audio::WaveFile<TestType> w("data/wave_files/tmp.wav");
        w.write(signal, audio::WaveFormat::IEEE_FLOAT, audio::WaveDataType::DOUBLE);

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
}
