#pragma once

#include "../reader.hpp"
#include <climits>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <tnt/dsp/multisignal.hpp>
#include <vector>

// Assert that "double" is 64 bits and "float" is 32 bits
static_assert(std::numeric_limits<double>::is_iec559);
static_assert(std::numeric_limits<float>::is_iec559);

namespace tnt::audio
{

template <typename T>
class wav_reader final : public reader<T>
{
public:

    explicit wav_reader(const std::filesystem::path& path)
        : m_file(path)
        , m_format_header()
        , m_format()
        , m_data_header()
        , m_data_begin()
        , m_initialized()
    {
        if (!m_file.is_open())
        {
            //TODO: error
            throw;
        }

        header riff_header;
        m_file.read(reinterpret_cast<char*>(&riff_header), sizeof(riff_header));
        //TODO: validate Riff header

        char wave[4];
        m_file.read(reinterpret_cast<char*>(&wave), sizeof(wave));
        //TODO: validate wave

        m_file.read(reinterpret_cast<char*>(&m_format_header), sizeof(m_format_header));
        //TODO: validate fmt

        std::vector<std::byte> buffer(m_format_header.size);
        m_file.read(reinterpret_cast<char*>(buffer.data()), m_format_header.size);
        std::memcpy(&m_format, buffer.data(), sizeof(m_format));
        //TODOL validate fmt contents

        // Keep reading until the start of the data chunk
        while (!m_file.eof())
        {
            header current_header;
            m_file.read(reinterpret_cast<char*>(&current_header), sizeof(header));
            if (!std::strncmp(current_header.id, "data", 4))
            {
                m_data_header = current_header;
                m_data_begin = m_file.tellg();
                m_initialized = true;
                break;
            }

            m_file.ignore(current_header.size);
        }

        if (!m_initialized)
        {
            //TODO: Failed to read metadata
            throw;
        }
    }

    virtual ~wav_reader() override = default;

    virtual size_t sample_rate() const override
    {
        assert(m_initialized);
        return m_format.sample_rate;
    }

    virtual size_t size() const override
    {
        assert(m_initialized);
        return m_data_header.size / this->channels() / (m_format.bits_per_sample / CHAR_BIT);
    }

    virtual size_t channels() const override
    {
        assert(m_initialized);
        return m_format.channels;
    }

    virtual bool eof() const override
    {
        return m_file.eof();
    }

    virtual void read(audio::multisignal<T>* buffer) override
    {
        assert(m_initialized);
        assert(m_file.is_open());

        std::for_each(buffer->begin(), buffer->end(), [this](auto& samples) {
            std::for_each(samples.begin(), samples.end(), [this](auto& sample) {
                if (m_format.bits_per_sample == sizeof(double) * CHAR_BIT)
                {
                    double value{};
                    m_file.read(reinterpret_cast<char*>(&value), sizeof(value));
                    sample = static_cast<T>(value);
                }
                else
                {
                    //TODO: error
                    throw;
                }
            });
        });

        //return buffer;

        ////    //TODO: other resolutions
        ////    if (m_header.bitsPerSample == 16)
        ////    {
        ////        auto value = static_cast<uintmax_t>(buffer[block] << 8) + static_cast<uintmax_t>(buffer[block + 1]);
        ////        signal[index] = (~value + 1) / static_cast<T>(65536);
        ////    }
        ////    else
        ////    {
        ////        //TODO: Error
        ////    }
        //}

        //return signal;
    }

private:

    struct header
    {
        char id[4];
        uint32_t size;
    };

    struct format
    {
        uint16_t format;
        uint16_t channels;
        uint32_t sample_rate;
        uint32_t byte_rate;
        uint16_t block_align;
        uint16_t bits_per_sample;
    };

    std::ifstream m_file;
    header m_format_header;
    format m_format;
    header m_data_header;
    std::streampos m_data_begin;
    bool m_initialized;
};

} /* namespace tnt::audio */