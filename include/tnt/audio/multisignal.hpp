#pragma once

#include <tnt/dsp/multisignal.hpp>

namespace tnt::audio
{

// Alias to dsp::signal
template <typename T>
using multisignal = dsp::multisignal<T>;

} /* namespace tnt::audio */