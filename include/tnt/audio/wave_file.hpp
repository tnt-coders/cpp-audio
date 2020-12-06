#pragma once

#include "file.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <sstream>

// Assert that data types are the correct size
static_assert(sizeof(uint8_t)* CHAR_BIT == 8);
static_assert(sizeof(uint16_t)* CHAR_BIT == 16);
// no 24 bit built in type
static_assert(sizeof(float)* CHAR_BIT == 32);
static_assert(sizeof(double)* CHAR_BIT == 64);

namespace tnt::audio
{

enum class wave_format
{
    PCM = 0x0001,
    IEEE_FLOAT = 0x0003,
};

enum class wave_data_type
{
    UINT8,
    INT16,
    INT24,
    INT32,
    FLOAT,
    DOUBLE,
};

template <typename T>
class wave_file final : public file<T>
{
public:

    explicit wave_file(const std::filesystem::path& path)
        : m_path(path)
        , m_sample_rate{}
        , m_size{}
        , m_channels{}
        , m_format{}
        , m_data_type{}
        , m_data_position{}
        , m_initialized{}
    {
        // If the file doesn't exist, return
        if (!std::filesystem::exists(path))
        {
            return;
        }

        this->initialize();
    }

    virtual ~wave_file() override = default;

    virtual bool initialized() const override
    {
        return m_initialized;
    }

    virtual double duration() const override
    {
        assert(this->initialized());
        return this->size() / static_cast<double>(this->sample_rate());
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
        std::ifstream file(m_path, std::ios::binary);
        if (!this->initialized() || !file.is_open())
        {
            throw std::runtime_error("Failed to open wave_file '" + m_path.string() + "' for reading");
        }

        // Seek to the start of the data
        file.seekg(m_data_position);

        multisignal<T> buffer{ this->sample_rate(), this->size(), this->channels() };
        std::for_each(buffer.begin(), buffer.end(), [this, &file](auto& samples) {
            std::for_each(samples.begin(), samples.end(), [this, &file](auto& sample) {

                //TODO: support other formats
                switch (m_data_type)
                {
                    case wave_data_type::UINT8:
                    {
                        uint8_t value{};
                        file.read(reinterpret_cast<char*>(&value), sizeof(value));
                        constexpr auto scale = (static_cast<size_t>(std::numeric_limits<uint8_t>::max()) + 1) / 2;
                        sample = (static_cast<T>(value) - scale) / scale;
                        break;
                    }
                    case wave_data_type::INT16:
                    {
                        int16_t value{};
                        file.read(reinterpret_cast<char*>(&value), sizeof(value));
                        constexpr auto scale = static_cast<size_t>(std::numeric_limits<int16_t>::max()) + 1;
                        sample = static_cast<T>(value) / scale;
                        break;
                    }
                    case wave_data_type::INT24:
                    {
                        // No built-in type for 24 bit data
                        constexpr auto int24_max = 0x7FFFFF;
                        std::byte bytes[3]{};

                        file.read(reinterpret_cast<char*>(&bytes), sizeof(bytes));
                        constexpr auto scale = int24_max + 1;

                        // Convert the bytes into a 32 bit value to make processing easier
                        int32_t value = static_cast<uint8_t>(bytes[0])
                                      + static_cast<uint8_t>(bytes[1]) * 0x100
                                      + static_cast<uint8_t>(bytes[2]) * 0x10000;

                        // If value > INT24_MAX
                        if (value > int24_max)
                        {
                            value -= 0x1000000;
                        }

                        sample = static_cast<T>(value) / scale;
                        break;
                    }
                    case wave_data_type::INT32:
                    {
                        int32_t value{};
                        file.read(reinterpret_cast<char*>(&value), sizeof(value));
                        constexpr auto scale = static_cast<size_t>(std::numeric_limits<int32_t>::max()) + 1;
                        sample = static_cast<T>(value) / scale;
                        break;
                    }
                    case wave_data_type::FLOAT:
                    {
                        float value{};
                        file.read(reinterpret_cast<char*>(&value), sizeof(value));
                        sample = static_cast<T>(value);
                        break;
                    }
                    case wave_data_type::DOUBLE:
                    {
                        double value{};
                        file.read(reinterpret_cast<char*>(&value), sizeof(value));
                        sample = static_cast<T>(value);
                        break;
                    }
                    default:
                    {
                        // The code should never get here
                        throw std::runtime_error("Your hair is on fire!");
                    }
                }
            });
        });

        return buffer;
    }

    virtual void write(const multisignal<T>& data) override
    {
        this->write(data, wave_format::IEEE_FLOAT, wave_data_type::DOUBLE);
    }

    virtual void write(const multisignal<T>& data, const wave_format& format)
    {
        switch (format)
        {
            case wave_format::PCM:
            {
                this->write(data, format, wave_data_type::INT32);
                break;
            }
            case wave_format::IEEE_FLOAT:
            {
                this->write(data, format, wave_data_type::DOUBLE);
                break;
            }
            default:
            {
                std::stringstream format_stream{};
                format_stream << std::hex << static_cast<size_t>(format);
                throw std::runtime_error("Invalid format '" + format_stream.str() + "' for wave_file '" + m_path.string() + "'");
            }
        }
    }

    virtual void write(const multisignal<T>& data, const wave_format& format, const wave_data_type& data_type)
    {
        std::ofstream file(m_path, std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open wave_file '" + m_path.string() + "' for writing");
        }

        // Write the RIFF header (size will be updated later)
        std::streampos riff_size_pos = file.tellp() + std::streamoff(sizeof(header::id));
        header riff_header = {
            { 'R', 'I', 'F', 'F' },
            0,
        };
        file.write(reinterpret_cast<char*>(&riff_header), sizeof(riff_header));

        // Write the fmt header (size will be updated later)
        std::streampos format_size_pos = file.tellp() + std::streamoff(sizeof(header::id));
        header format_header = {
            { 'f', 'm', 't', ' ' },
            sizeof(format_chunk),
        };
        

        switch (format)
        {
            case wave_format::PCM:
            {
                // Set PCM format extension
                format_ext_pcm format_ext{};

                switch (data_type)
                {
                    case wave_data_type::UINT8:
                    {
                        format_ext.bits_per_sample = 8;
                        break;
                    }
                    case wave_data_type::INT16:
                    {
                        format_ext.bits_per_sample = 16;
                        break;
                    }
                    case wave_data_type::INT24:
                    {
                        format_ext.bits_per_sample = 24;
                        break;
                    }
                    case wave_data_type::INT32:
                    {
                        format_ext.bits_per_sample = 32;
                        break;
                    }
                    default:
                    {
                        //TODO: error
                        throw;
                        break;
                    }
                }

                break;
            }
            case wave_format::IEEE_FLOAT:
            {
                // Set IEEE_FLOAT format extension
                format_ext_ieee_float format_ext{};
                
                break;
            }
            default:
            {
                std::stringstream format_stream{};
                format_stream << std::hex << static_cast<size_t>(format);
                throw std::runtime_error("Invalid format '" + format_stream.str() + "' for wave_file '" + m_path.string() + "'");
            }
        }
    }

private:

    void initialize()
    {
        std::ifstream file(m_path, std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open wave_file '" + m_path.string() + "'");
        }

        header riff_header{};
        file.read(reinterpret_cast<char*>(&riff_header), sizeof(riff_header));
        if (std::strncmp(riff_header.id, "RIFF", sizeof(riff_header.id)))
        {
            throw std::runtime_error("Invalid RIFF header for wave_file '" + m_path.string() + "'");
        }

        char wave[4]{};
        file.read(reinterpret_cast<char*>(&wave), sizeof(wave));
        if (std::strncmp(wave, "WAVE", sizeof(wave)))
        {
            throw std::runtime_error("Invalid RIFF format for wave_file '" + m_path.string() + "'");
        }

        header format_header{};
        file.read(reinterpret_cast<char*>(&format_header), sizeof(format_header));
        if (std::strncmp(format_header.id, "fmt ", sizeof(format_header.id)))
        {
            throw std::runtime_error("Invalid fmt header for wave_file '" + m_path.string() + "'");
        }

        format_chunk format_chunk{};
        file.read(reinterpret_cast<char*>(&format_chunk), sizeof(format_chunk));
        size_t format_bytes_read = sizeof(format_chunk);
        if (file.fail())
        {
            throw std::runtime_error("Error reading format chunk for wave_file '" + m_path.string() + "'");
        }

        m_sample_rate = format_chunk.sample_rate;
        m_channels = format_chunk.channels;

        switch (format_chunk.format)
        {
            case wave_format::PCM:
            {
                m_format = wave_format::PCM;

                // Read PCM format extension
                format_ext_pcm format_ext{};
                file.read(reinterpret_cast<char*>(&format_ext), sizeof(format_ext));
                if (file.fail())
                {
                    throw std::runtime_error("Error reading PCM format extension for wave_file '" + m_path.string() + "'");
                }

                format_bytes_read += sizeof(format_ext);

                switch (format_ext.bits_per_sample)
                {
                    case 8:
                    {
                        m_data_type = wave_data_type::UINT8;
                        break;
                    }
                    case 16:
                    {
                        m_data_type = wave_data_type::INT16;
                        break;
                    }
                    case 24:
                    {
                        m_data_type = wave_data_type::INT24;
                        break;
                    }
                    case 32:
                    {
                        m_data_type = wave_data_type::INT32;
                        break;
                    }
                    default:
                    {
                        throw std::runtime_error("Invalid PCM data type for wave_file '" + m_path.string() + "'");
                    }
                }

                break;
            }
            case wave_format::IEEE_FLOAT:
            {
                m_format = wave_format::IEEE_FLOAT;

                // Read IEEE_FLOAT format extension
                format_ext_ieee_float format_ext{};
                file.read(reinterpret_cast<char*>(&format_ext), sizeof(format_ext));
                if (file.fail())
                {
                    throw std::runtime_error("Error reading IEEE_FLOAT format extension for wave_file '" + m_path.string() + "'");
                }

                format_bytes_read += sizeof(format_ext);

                switch (format_ext.bits_per_sample)
                {
                    case 32:
                    {
                        m_data_type = wave_data_type::FLOAT;
                        break;
                    }
                    case 64:
                    {
                        m_data_type = wave_data_type::DOUBLE;
                        break;
                    }
                    default:
                    {
                        throw std::runtime_error("Invalid IEEE_FLOAT data type for wave_file '" + m_path.string() + "'");
                    }
                }

                break;
            }
            default:
            {
                std::stringstream format_stream{};
                format_stream << std::hex << format_chunk.format;
                throw std::runtime_error("Invalid format '" + format_stream.str() + "' for wave_file '" + m_path.string() + "'");
            }
        }

        // Ignore any extra data at the end of the format chunk
        if (format_header.size != format_bytes_read)
        {
            file.ignore(format_header.size - format_bytes_read);
            if (file.fail())
            {
                throw std::runtime_error("Unexpected fmt chunk size for wave_file '" + m_path.string() + "'");
            }
        }

        // Keep reading until the start of the data chunk
        while (!file.eof())
        {
            header current_header{};
            file.read(reinterpret_cast<char*>(&current_header), sizeof(current_header));
            if (file.fail())
            {
                throw std::runtime_error("Invalid chunk header found in wave_file '" + m_path.string() + "'");
            }

            if (!std::strncmp(current_header.id, "data", 4))
            {
                m_data_position = file.tellg();
                m_size = current_header.size / format_chunk.block_align;
                m_initialized = true;
                break;
            }

            file.ignore(current_header.size);
            if (file.fail())
            {
                if (file.eof())
                {
                    throw std::runtime_error("Unexpected EOF for wave_file '" + m_path.string() + "'");
                }
                else
                {
                    throw std::runtime_error("Failed to ignore chunk " + std::string(current_header.id) + " in wave_file '" + m_path.string() + "'");
                }
            }
        }

        if (!this->initialized())
        {
            throw std::runtime_error("Failed to initialize wave_file '" + m_path.string() + "'");
        }
    }

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

    std::filesystem::path m_path;
    wave_format m_format;
    wave_data_type m_data_type;
    size_t m_sample_rate;
    size_t m_size;
    size_t m_channels;
    std::streampos m_data_position;
    bool m_initialized;
};

} /* namespace tnt::audio */