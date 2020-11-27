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
    \brief 
    */
    virtual bool initialized() const = 0;

    virtual size_t sample_rate() const = 0;

    virtual size_t duration() const = 0;

    virtual size_t size() const = 0;

    virtual size_t channels() const = 0;

    virtual multisignal<T> read() = 0;

    virtual void write(const multisignal<T>& data) = 0;
};

// Even abstract destructors need a definition
template<typename T>
file_base<T>::~file_base() = default;

} /* namespace tnt::audio */
