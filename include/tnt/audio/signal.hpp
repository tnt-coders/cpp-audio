#pragma once

#include <tnt/dsp/signal.hpp>

namespace tnt::audio
{

/*!
\brief Represents an audio signal to store and process audio data
*/
template <typename T>
using Signal = dsp::Signal<T>;

}  // namespace tnt::audio
