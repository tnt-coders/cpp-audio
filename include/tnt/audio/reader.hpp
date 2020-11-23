#pragma once

#include "multisignal.hpp"

namespace tnt::audio
{

template <typename T>
class reader
{
public:
    virtual ~reader() = 0;

    virtual size_t sample_rate() const = 0;

    virtual size_t size() const = 0;

    virtual size_t channels() const = 0;

    virtual bool eof() const = 0;

    virtual void read(multisignal<T>* buffer) = 0;

    multisignal<T> buffer(size_t count) const
    {
        return multisignal<T>{ this->sample_rate(), count, this->channels() };
    }
};

// Even abstract destructors need a definition
template <typename T>
reader<T>::~reader() = default;

} /* namespace tnt::audio */