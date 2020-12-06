#include "approx.hpp"
#include "config.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <tnt/audio/wave_file.hpp>
#include <tnt/dsp/multisignal.hpp>
#include <tnt/dsp/signal.hpp>
#include <tnt/dsp/signal_generator.hpp>

using namespace tnt;

//TODO: Make more flexible so different channels can have different signal types
template <typename T>
dsp::multisignal<T> signal_from_config(const std::filesystem::path& path)
{
    const auto config = read_config(path);
    const auto type = config.at("type");
    const auto sample_rate = std::stoull(config.at("sample_rate"));
    const auto frequency = static_cast<T>(std::stod(config.at("frequency")));
    const auto channels = std::stoull(config.at("channels"));
    const auto size = std::stoull(config.at("size"));
    const auto g = dsp::signal_generator<T>(sample_rate, size);

    dsp::multisignal<T> signal(sample_rate, size);
    if (type == "cos")
    {
        for (size_t c = 0; c < channels; ++c)
        {
            signal.add_channel(g.cosine(frequency));
        }
        return signal;
    }
    else if (type == "sin")
    {
        for (size_t c = 0; c < channels; ++c)
        {
            signal.add_channel(g.sine(frequency));
        }
        return signal;
    }
    else
    {
        throw std::runtime_error("Invalid config value for 'type' in" + path.string());
    }
}

TEMPLATE_TEST_CASE("wave_file construction", "[file][wave_file][constructor]", float, double)
{
    SECTION("PCM_U8")
    {
        audio::wave_file<TestType> w("data/wave_files/PCM_U8.wav");
        CHECK(w.initialized());
    }

    SECTION("PCM_16")
    {
        audio::wave_file<TestType> w("data/wave_files/PCM_16.wav");
        CHECK(w.initialized());
    }

    SECTION("PCM_24")
    {
        audio::wave_file<TestType> w("data/wave_files/PCM_24.wav");
        CHECK(w.initialized());
    }

    SECTION("PCM_32")
    {
        audio::wave_file<TestType> w("data/wave_files/PCM_32.wav");
        CHECK(w.initialized());
    }

    SECTION("FLOAT")
    {
        audio::wave_file<TestType> w("data/wave_files/FLOAT.wav");
        CHECK(w.initialized());
    }

    SECTION("DOUBLE")
    {
        audio::wave_file<TestType> w("data/wave_files/DOUBLE.wav");
        CHECK(w.initialized());
    }

    SECTION("Invalid file format (empty)")
    {
        REQUIRE_THROWS_WITH(
            audio::wave_file<TestType>("data/wave_files/empty.wav"),
            Catch::Matchers::Equals("Invalid RIFF header for wave_file 'data/wave_files/empty.wav'")
        );
    }

    SECTION("File doesn't exist")
    {
        audio::wave_file<TestType> w("data/wave_files/nonexistent.wav");
        CHECK(!w.initialized());
    }
}

TEMPLATE_TEST_CASE("wave_file accessors", "[file][wave_file][accessors]", float, double)
{
    const auto signal = signal_from_config<TestType>("data/wave_files/signal.dat");
    audio::wave_file<TestType> w("data/wave_files/DOUBLE.wav");

    CHECK(w.initialized());
    CHECK(w.duration() == signal.duration());
    CHECK(w.sample_rate() == signal.sample_rate());
    CHECK(w.size() == signal.size());
    CHECK(w.channels() == signal.channels());
}

TEMPLATE_TEST_CASE("wave_file::read", "[file][wave_file][read]", float, double)
{
    const auto signal = signal_from_config<TestType>("data/wave_files/signal.dat");

    SECTION("PCM_U8")
    {
        constexpr auto scale = (static_cast<size_t>(std::numeric_limits<uint8_t>::max()) + 1) / 2;
        constexpr auto epsilon = static_cast<TestType>(1) / scale;

        audio::wave_file<TestType> w("data/wave_files/PCM_U8.wav");
        const auto s = w.read();

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
        constexpr auto scale = static_cast<size_t>(std::numeric_limits<int16_t>::max() + 1);
        constexpr auto epsilon = static_cast<TestType>(1) / scale;

        audio::wave_file<TestType> w("data/wave_files/PCM_16.wav");
        const auto s = w.read();

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
        // No need to use "epsilon" anymore because 24 bit data is more accurate

        audio::wave_file<TestType> w("data/wave_files/PCM_24.wav");
        const auto s = w.read();

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
        audio::wave_file<TestType> w("data/wave_files/PCM_32.wav");
        const auto s = w.read();

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
        audio::wave_file<TestType> w("data/wave_files/FLOAT.wav");
        const auto s = w.read();

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
        audio::wave_file<TestType> w("data/wave_files/DOUBLE.wav");
        const auto s = w.read();

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

    //TODO: more formats

    SECTION("File doesn't exist")
    {
        audio::wave_file<TestType> w("data/wave_files/nonexistent.wav");

        REQUIRE_THROWS_WITH(
            w.read(),
            Catch::Matchers::Equals("Failed to open wave_file 'data/wave_files/nonexistent.wav' for reading")
        );
    }
}

TEMPLATE_TEST_CASE("wave_file::write", "[file][wave_file][write]", float, double)
{
    const auto signal = signal_from_config<TestType>("data/wave_files/signal.dat");

    SECTION("PCM_U8")
    {
        constexpr auto scale = (static_cast<size_t>(std::numeric_limits<uint8_t>::max()) + 1) / 2;
        constexpr auto epsilon = static_cast<TestType>(1) / scale;

        audio::wave_file<TestType> w("data/wave_files/tmp.wav");
        w.write(signal, audio::wave_format::PCM, audio::wave_data_type::UINT8);
        const auto s = w.read();

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
}