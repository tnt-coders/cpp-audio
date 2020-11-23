#pragma once

#include <tnt/dsp/signal.hpp>

namespace tnt::audio
{

// Alias to dsp::signal
template <typename T>
using signal = dsp::signal<T>;

} /* namespace tnt::audio */