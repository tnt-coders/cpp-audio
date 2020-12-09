#pragma once

#include "multisignal.hpp"

namespace tnt::audio
{

/*!
\brief File object used to read/write audio data
*/
template <typename T>
class file
{
public:

    /*!
    \brief Destructor
    */
    virtual ~file() = 0;

    virtual double duration() = 0;

    virtual size_t sample_rate() = 0;

    virtual size_t size() = 0;

    virtual size_t channels() = 0;

    virtual multisignal<T> read() = 0;

    virtual void write(const multisignal<T>& data) = 0;
};

// Even abstract destructors need a definition
template<typename T>
file<T>::~file() = default;

} /* namespace tnt::audio */
