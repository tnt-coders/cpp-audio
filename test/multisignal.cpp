#include <catch2/catch_template_test_macros.hpp>
#include <tnt/audio/multisignal.hpp>

using namespace tnt;

TEMPLATE_TEST_CASE("Multisignal conversion", "[Multisignal][conversion]", double, float)
{
    audio::Multisignal<TestType> x{ 1000 };

    // Check that audio::multisignals can be passed to functions that take dsp::multisignals
    CHECK(multisignal_conversion(x));
}

template <typename T>
bool multisignal_conversion(const dsp::Multisignal<T>& signal)
{
    return true;
}