#pragma once

#include "../file_base.hpp"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>

// Assert that "double" is 64 bits and "float" is 32 bits
static_assert(std::numeric_limits<double>::is_iec559);
static_assert(std::numeric_limits<float>::is_iec559);

namespace tnt::audio::impl
{

template <typename T>
class wav_file final : public file_base<T>
{
public:

    explicit wav_file(const std::filesystem::path& path)
        : m_file(path)
        , m_sample_rate()
        , m_size()
        , m_channels()
        , m_data_type()
        , m_data_position()
        , m_initialized()
    {
        if (!m_file.is_open())
        {
            //TODO: error
            throw;
        }

        this->initialize();
    }

    virtual ~wav_file() override = default;

    virtual bool initialized() const override
    {
        return m_initialized;
    }

    virtual size_t sample_rate() const override
    {
        assert(this->initialized());
        return m_sample_rate;
    }

    virtual size_t size() const override
    {
        assert(this->initialized());
        return m_size;
    }

    virtual size_t channels() const override
    {
        assert(this->initialized());
        return m_channels;
    }

    virtual multisignal<T> read() override
    {
        assert(m_initialized);
        assert(m_file.is_open());

        // Seek to the start of the data
        m_file.seekg(m_data_position);

        multisignal<T> buffer{ this->sample_rate(), this->size(), this->channels() };
        std::for_each(buffer.begin(), buffer.end(), [this](auto& samples) {
            std::for_each(samples.begin(), samples.end(), [this](auto& sample) {

                //TODO: support other formats

                if (m_data_type == data_type::DOUBLE)
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

        return buffer;

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

    virtual void write(const multisignal<T>& data) override
    {

    }

private:

    void initialize()
    {
        header riff_header;
        m_file.read(reinterpret_cast<char*>(&riff_header), sizeof(riff_header));
        //TODO: validate Riff header

        char wave[4];
        m_file.read(reinterpret_cast<char*>(&wave), sizeof(wave));
        //TODO: validate wave

        header format_header;
        m_file.read(reinterpret_cast<char*>(&format_header), sizeof(format_header));
        //TODO: validate fmt

        // Sometimes the size of the format chunk doesn't match the struct so read into a buffer
        std::vector<std::byte> buffer(format_header.size);
        m_file.read(reinterpret_cast<char*>(buffer.data()), format_header.size);

        format format_chunk;
        std::memcpy(&format_chunk, buffer.data(), sizeof(format_chunk));
        //TODOL validate fmt contents

        m_sample_rate = format_chunk.sample_rate;
        m_channels = format_chunk.channels;

        // Determine the data type
        switch (format_chunk.bits_per_sample)
        {
        case sizeof(double) * CHAR_BIT:
            m_data_type = data_type::DOUBLE;
            break;
        default:
            //TODO: error, unsupported datatype
            break;
        }

        // Keep reading until the start of the data chunk
        while (!m_file.eof())
        {
            header current_header;
            m_file.read(reinterpret_cast<char*>(&current_header), sizeof(header));
            if (!std::strncmp(current_header.id, "data", 4))
            {
                m_data_position = m_file.tellg();
                m_size = current_header.size / format_chunk.channels / (format_chunk.bits_per_sample / CHAR_BIT);
                m_initialized = true;
                break;
            }

            m_file.ignore(current_header.size);
        }

        if (!this->initialized())
        {
            //TODO: Failed to read metadata
            throw;
        }
    }

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

    enum class data_type
    {
        DOUBLE,
    };

    std::ifstream m_file;
    size_t m_sample_rate;
    size_t m_size;
    size_t m_channels;
    data_type m_data_type;
    std::streampos m_data_position;
    bool m_initialized;
};

} /* namespace tnt::audio::file */