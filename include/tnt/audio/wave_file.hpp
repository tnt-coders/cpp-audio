#pragma once

#include "file_base.hpp"
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>

// Assert that data types are the correct size
static_assert(sizeof(uint8_t)* CHAR_BIT == 8);
static_assert(sizeof(uint16_t)* CHAR_BIT == 16);
// no 24 bit built in type
static_assert(sizeof(float)* CHAR_BIT == 32);
static_assert(sizeof(double)* CHAR_BIT == 64);

namespace tnt::audio
{

template <typename T>
class wave_file final : public file_base<T>
{
public:

    explicit wave_file(const std::filesystem::path& path)
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

    virtual ~wave_file() override = default;

    virtual bool initialized() const override
    {
        return m_initialized;
    }

    virtual size_t sample_rate() const override
    {
        assert(this->initialized());
        return m_sample_rate;
    }

    virtual size_t duration() const override
    {
        assert(this->initialized());
        //TODO: implement
        throw std::runtime_error("Not implemented yet");
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
                switch (m_data_type)
                {
                    case data_type::PCM_U8:
                    {
                        //TODO: implement (slightly more complicated because it is unsigned)
                        throw std::runtime_error("Not implemented yet");
                    }
                    case data_type::PCM_16:
                    {
                        int16_t value{};
                        m_file.read(reinterpret_cast<char*>(&value), sizeof(value));
                        sample = static_cast<T>(value) / std::numeric_limits<int16_t>::max();
                    }
                    case data_type::PCM_24:
                    {
                        //TODO: implement (no built in type so this is a bit more difficult)
                        throw std::runtime_error("Not implemented yet");
                    }
                    case data_type::PCM_32:
                    {
                        int32_t value{};
                        m_file.read(reinterpret_cast<char*>(&value), sizeof(value));
                        sample = static_cast<T>(value) / std::numeric_limits<int32_t>::max();
                    }
                    case data_type::FLOAT:
                    {
                        float value{};
                        m_file.read(reinterpret_cast<char*>(&value), sizeof(value));
                        sample = static_cast<T>(value);
                    }
                    case data_type::DOUBLE:
                    {
                        double value{};
                        m_file.read(reinterpret_cast<char*>(&value), sizeof(value));
                        sample = static_cast<T>(value);
                    }
                    default:
                    {
                        //TODO: invalid format (should never get here)
                        throw;
                    }
                }
            });
        });

        return buffer;
    }

    virtual void write(const multisignal<T>& data) override
    {

    }

private:

    void initialize()
    {
        header riff_header{};
        m_file.read(reinterpret_cast<char*>(&riff_header), sizeof(riff_header));
        //TODO: validate Riff header

        char wave[4]{};
        m_file.read(reinterpret_cast<char*>(&wave), sizeof(wave));
        //TODO: validate wave

        header format_header{};
        m_file.read(reinterpret_cast<char*>(&format_header), sizeof(format_header));
        //TODO: validate format header

        format_chunk format{};
        m_file.read(reinterpret_cast<char*>(&format), sizeof(format));
        size_t format_bytes_read = sizeof(format);
        //TODO: validate format chunk

        m_sample_rate = format.sample_rate;
        m_channels = format.channels;

        switch (format.format)
        {
            case file_type::PCM:
            {
                m_file_type = file_type::PCM;

                // Read PCM format extension
                format_ext_pcm format_ext;
                m_file.read(reinterpret_cast<char*>(&format_ext), sizeof(format_ext));
                format_bytes_read += sizeof(format_ext);

                switch (format_ext.bits_per_sample)
                {
                    case 8:
                    {
                        m_data_type = data_type::PCM_U8;
                        break;
                    }
                    case 16:
                    {
                        m_data_type = data_type::PCM_16;
                        break;
                    }
                    case 24:
                    {
                        m_data_type = data_type::PCM_24;
                        break;
                    }
                    case 32:
                    {
                        m_data_type = data_type::PCM_32;
                        break;
                    }
                    default:
                    {
                        //TODO: invalid format
                        throw;
                    }
                }

                break;
            }
            case file_type::IEEE_FLOAT:
            {
                m_file_type = file_type::IEEE_FLOAT;

                // Read IEEE_FLOAT format extension
                format_ext_ieee_float format_ext{};
                m_file.read(reinterpret_cast<char*>(&format_ext), sizeof(format_ext));
                format_bytes_read += sizeof(format_ext);

                switch (format_ext.bits_per_sample)
                {
                    case 32:
                    {
                        m_data_type = data_type::FLOAT;
                        break;
                    }
                    case 64:
                    {
                        m_data_type = data_type::DOUBLE;
                        break;
                    }
                    default:
                    {
                        //TODO: invalid format
                        throw;
                    }
                }

                break;
            }
            default:
            {
                //TODO: invalid format
                throw;
            }
        }

        // Ignore any extra data at the end of the format chunk
        if (format_header.size != format_bytes_read)
        {
            m_file.ignore(format_header.size - format_bytes_read);
        }

        // Keep reading until the start of the data chunk
        while (!m_file.eof())
        {
            header current_header{};
            m_file.read(reinterpret_cast<char*>(&current_header), sizeof(header));
            if (!std::strncmp(current_header.id, "data", 4))
            {
                m_data_position = m_file.tellg();
                m_size = current_header.size / format.block_align;
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

    enum class file_type
    {
        PCM = 0x0001,
        IEEE_FLOAT = 0x0003,
    };

    enum class data_type
    {
        PCM_U8,
        PCM_16,
        PCM_24,
        PCM_32,
        FLOAT,
        DOUBLE,
    };

// Do NOT allow padding of the structures used for reading data
#pragma pack(push, 1)
    struct header
    {
        char id[4];
        uint32_t size;
    };

    struct format_chunk
    {
        uint16_t format;
        uint16_t channels;
        uint32_t sample_rate;
        uint32_t byte_rate;
        uint16_t block_align;
    };

    struct format_ext_pcm
    {
        uint16_t bits_per_sample;
    };

    struct format_ext_ieee_float
    {
        uint16_t bits_per_sample;

        // The internet suggests this extra field should exist, but in practice it is sometimes missing
        //uint16_t extension_size;
    };
#pragma pack(pop)

    std::ifstream m_file;
    file_type m_file_type;
    data_type m_data_type;
    size_t m_sample_rate;
    size_t m_size;
    size_t m_channels;
    std::streampos m_data_position;
    bool m_initialized;
};

} /* namespace tnt::audio */