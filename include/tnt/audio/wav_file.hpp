#pragma once

#include "file.hpp"
#include <climits>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <tnt/dsp/multi_signal.hpp>
#include <tnt/dsp/signal.hpp>

#include <iostream>

// Assert that "double" is 64 bits
static_assert(std::numeric_limits<double>::is_iec559);

// Assert that "float" is 32 bits
static_assert(std::numeric_limits<float>::is_iec559);

namespace tnt::audio
{

template <typename T>
class WavFile final : public File<T>
{
public:

    explicit WavFile(const std::filesystem::path& path)
        : File<T>(path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            //TODO: error
        }

        file.read(reinterpret_cast<char*>(&m_riffHeader), sizeof(Header));
        //TODO: validate Riff header

        char wave[4];
        file.read(reinterpret_cast<char*>(&wave), sizeof(wave));
        //TODO: validate wave

        file.read(reinterpret_cast<char*>(&m_formatHeader), sizeof(Header));
        //TODO: validate fmt

        std::vector<std::byte> buffer(m_formatHeader.size);
        file.read(reinterpret_cast<char*>(buffer.data()), m_formatHeader.size);

        std::memcpy(&m_format, buffer.data(), sizeof(m_format));

        // Set attributes of the parent object
        this->SetChannels(m_format.channels);
        this->SetSampleRate(m_format.sampleRate);

        // Find the start of the data chunk
        bool found = false;
        while (!file.eof())
        {
            Header header;
            file.read(reinterpret_cast<char*>(&header), sizeof(Header));
            if (!std::strncmp(header.id, "data", 4))
            {
                m_dataHeader = header;
                m_dataBegin = file.tellg();
                found = true;
                break;
            }

            file.ignore(header.size);
        }

        if (!found)
        {
            //TODO: Error if data chunk is not found
        }
    }

    virtual dsp::MultiSignal<T> Read() const override
    {
        const auto path = this->GetPath();

        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            //TODO: error
        }

        // Read all channels into a buffer
        std::vector<std::byte> buffer(m_dataHeader.size);
        file.seekg(m_dataBegin);
        file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

        // Get the specific channel out of the buffer
        const auto sampleRate = m_format.sampleRate;
        const auto size = m_dataHeader.size / m_format.channels / CHAR_BIT;
        const auto channels = m_format.channels;
        dsp::MultiSignal<T> signal{ sampleRate, size, channels };
        for (size_t index = 0; index < signal.size(); ++index)
        {
            const auto block = index * m_format.blockAlign;
            for (size_t channel = 0; channel < m_format.channels; ++channel)
            {
                if (m_format.bitsPerSample == sizeof(double) * CHAR_BIT)
                {
                    size_t offset = channel * m_format.bitsPerSample / CHAR_BIT;
                    double value;
                    std::memcpy(&value, &buffer[block + offset], sizeof(value));
                    signal[channel][index] = static_cast<T>(value);
                }
                else
                {
                    //TODO: error
                }
            }
            
        //    //TODO: other resolutions
        //    if (m_header.bitsPerSample == 16)
        //    {
        //        auto value = static_cast<uintmax_t>(buffer[block] << 8) + static_cast<uintmax_t>(buffer[block + 1]);
        //        signal[index] = (~value + 1) / static_cast<T>(65536);
        //    }
        //    else
        //    {
        //        //TODO: Error
        //    }
        }

        return signal;
    }

private:

    struct Header
    {
        char id[4];
        uint32_t size;
    };

    struct Format
    {
        uint16_t format;
        uint16_t channels;
        uint32_t sampleRate;
        uint32_t byteRate;
        uint16_t blockAlign;
        uint16_t bitsPerSample;
    };

    Header m_riffHeader;
    Header m_formatHeader;
    Header m_dataHeader;

    Format m_format;
    std::streampos m_dataBegin;
};

} /* namespace tnt::audio */