#include <catch2/catch_template_test_macros.hpp>
#include <tnt/audio/signal.hpp>
#include <tnt/dsp/signal.hpp>

using namespace tnt;

TEMPLATE_TEST_CASE("signal conversion", "[signal][conversion]", double, float)
{
    audio::signal<TestType> x(1000);

    // Check that audio::signals can be passed to functions that take dsp::signals
    CHECK(signal_conversion(x));
}

template <typename T>
bool signal_conversion(const dsp::Signal<T>& signal)
{
    return true;
}
