#pragma once

#include <tnt/dsp/multisignal.hpp>

namespace tnt::audio
{

/*!
\brief Represents a multi-channel audio signal to store and process audio data
*/
template <typename T>
using multisignal = dsp::Multisignal<T>;

}  // namespace tnt::audio
