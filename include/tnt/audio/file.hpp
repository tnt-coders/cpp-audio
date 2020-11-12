#pragma once

#include <filesystem>
#include <tnt/dsp/multi_signal.hpp>

namespace tnt::audio
{

template <typename T>
class File
{
public:

    explicit File(const std::filesystem::path& path, const size_t sampleRate = 0, const size_t channels = 0)
        : m_path(path)
        , m_sampleRate(sampleRate)
        , m_channels(channels)
    {}

    virtual ~File() = 0;

    size_t GetChannels() const
    {
        return m_channels;
    }

    size_t GetSampleRate() const
    {
        return m_sampleRate;
    }

    virtual dsp::MultiSignal<T> Read() const = 0;

protected:

    std::filesystem::path GetPath() const
    {
        return m_path;
    }

    void SetChannels(const size_t channels)
    {
        m_channels = channels;
    }

    void SetSampleRate(const size_t sampleRate)
    {
        m_sampleRate = sampleRate;
    }

private:

    std::filesystem::path m_path;
    size_t m_channels;
    size_t m_sampleRate;
};

// Even pure virtual destructors require a definition
template <typename T>
File<T>::~File() = default;

} /* namespace tnt::audio */