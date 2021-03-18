#pragma once

#include "multisignal.hpp"

namespace tnt::audio
{

/*!
\brief File object used to read/write audio data
*/
template <typename T>
class file_base
{
public:
    /*!
    \brief Destructor
    */
    virtual ~file_base() = 0;

    /*!
    \brief Gets the duration of the audio file in seconds
    \return Duration
    */
    virtual double duration() = 0;

    /*!
    \brief Gets the sample rate of the encoded audio data
    \return Sample Rate
    */
    virtual size_t sample_rate() = 0;

    /*!
    \brief Gets the number of samples in a single channel of the contained audio data
    \return Size
    */
    virtual size_t size() = 0;

    /*!
    \brief Gets the number of channels contained in the audio file
    \return Channels
    */
    virtual size_t channels() = 0;

    /*!
    \brief Reads the audio data from the file
    \return Multi-channel signal containing the audio data
    */
    virtual multisignal<T> read() = 0;

    /*!
    \brief Writes audio data to the file
    \param[in] signal Multi-channel signal containing audio data to write to the file
    */
    virtual void write(const multisignal<T>& signal) = 0;
};

// Even abstract destructors need a definition
template <typename T>
file_base<T>::~file_base() = default;

}  // namespace tnt::audio
