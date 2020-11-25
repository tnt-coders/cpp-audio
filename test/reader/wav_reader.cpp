#include "../approx.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <tnt/audio/multisignal.hpp>
//#include <tnt/audio/reader/wav_reader.hpp>
#include <tnt/dsp/signal_generator.hpp>

using namespace tnt;

TEMPLATE_TEST_CASE("wav_reader construction", "[reader][wav_reader][construction]", double, float)
{
    /*audio::wav_reader<TestType> r("sinewave_64bit_100Hz_1000Hz-sample-rate_10-size_1-channels.wav");
    CHECK(r.sample_rate() == 1000);
    CHECK(r.size() == 10);
    CHECK(r.channels() == 1);*/
}

TEMPLATE_TEST_CASE("wav_reader::read", "[reader][wav_reader][read]", double, float)
{
    //audio::wav_reader<TestType> r("sinewave_64bit_100Hz_1000Hz-sample-rate_10-size_1-channels.wav");
    //dsp::signal_generator<TestType> g{ r.sample_rate(), r.size() };
    //const auto sine = g.sine(100);
    //auto buffer = r.buffer(r.size() / 2);

    //size_t offset = 0;
    //while (!r.eof())
    //{
    //    r.read(&buffer);
    //    for (size_t n = 0; n < buffer.size(); ++n)
    //    {
    //        if (n + offset < sine.size())
    //        {
    //            CHECK(buffer[n][0] == approx(sine[n + offset]));
    //        }
    //        else
    //        {
    //            CHECK(buffer[n][0] == approx(0));
    //        }
    //    }

    //    offset += buffer.size();
    //}
}
