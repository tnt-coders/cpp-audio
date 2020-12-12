#pragma once

#include "file.hpp"
#include <array>
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

/*!
\brief Represents valid wave file formats
*/
enum class wave_format
{
    PCM = 0x0001,
    IEEE_FLOAT = 0x0003,
};

/*!
\brief Represents valid wave file data types
*/
enum class wave_data_type
{
    UINT8,
    INT16,
    INT24,
    INT32,
    FLOAT,
    DOUBLE,
};

/*!
\brief Wave file object used to read and write wave files
*/
template <typename T>
class wave_file final : public file<T>
{
public:

    /*!
    \brief Constructor
    \param[in] path Path to the wave file on the system
    */
    explicit wave_file(const std::filesystem::path& path)
        : m_path(path)
        , m_sample_rate{}
        , m_size{}
        , m_channels{}
        , m_format{}
        , m_data_type{}
        , m_data_position{}
        , m_initialized{}
    {}

    /*!
    \brief Destructor
    */
    virtual ~wave_file() override = default;

    /*!
    \copydoc file::duration()
    */
    virtual double duration() override
    {
        if (!m_initialized)
        {
            this->initialize();
        }

        return this->size() / static_cast<double>(this->sample_rate());
    }

    /*!
    \copydoc file::sample_rate()
    */
    virtual size_t sample_rate() override
    {
        if (!m_initialized)
        {
            this->initialize();
        }

        return m_sample_rate;
    }

    /*!
    \copydoc file::size()
    */
    virtual size_t size() override
    {
        if (!m_initialized)
        {
            this->initialize();
        }

        return m_size;
    }

    /*!
    \copydoc file::channels()
    */
    virtual size_t channels() override
    {
        if (!m_initialized)
        {
            this->initialize();
        }

        return m_channels;
    }

    /*!
    \copydoc file::read()
    */
    virtual multisignal<T> read() override
    {
        if (!m_initialized)
        {
            this->initialize();
        }

        std::ifstream file(m_path, std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open wave_file '" + m_path.string() + "' for reading");
        }

        // Seek to the start of the data
        file.seekg(m_data_position);

        multisignal<T> signal{ this->sample_rate(), this->size(), this->channels() };
        std::for_each(signal.begin(), signal.end(), [this, &file](auto& samples) {
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
                        constexpr auto uint24_max = 0x1000000;

                        // Store in an int32 because there is no built in 24 bit type
                        int32_t value{};

                        // Only read 3 bytes because it is a 24 bit value
                        file.read(reinterpret_cast<char*>(&value), 3);

                        // Get 32 bit integer from 24 bit two's complement value
                        if (value > int24_max)
                        {
                            value -= uint24_max;
                        }

                        constexpr auto scale = int24_max + 1;
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

        return signal;
    }

    /*!
    \copydoc file::write(const multisignal<T>& signal)
    */
    virtual void write(const multisignal<T>& signal) override
    {
        this->write(signal, wave_format::IEEE_FLOAT, wave_data_type::DOUBLE);
    }

    /*!
    \brief Writes a wave file in the specified format
    \param[in] signal Multi-channel signal containing audio data to write to the file
    \param[in] format Format to write the wave file in
    */
    virtual void write(const multisignal<T>& signal, const wave_format& format)
    {
        switch (format)
        {
            case wave_format::PCM:
            {
                this->write(signal, format, wave_data_type::INT32);
                break;
            }
            case wave_format::IEEE_FLOAT:
            {
                this->write(signal, format, wave_data_type::DOUBLE);
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

    /*!
    \brief Writes a wave file in the specified format with the specified data type
    \param[in] signal Multi-channel signal containing audio data to write to the file
    \param[in] format Format to write the wave file in
    \param[in] data_type Data type to store the data in
    */
    virtual void write(const multisignal<T>& signal, const wave_format& format, const wave_data_type& data_type)
    {
        header riff_header{};
        std::string("RIFF").copy(riff_header.id, sizeof(riff_header.id));

        char wave[4]{};
        std::string("WAVE").copy(wave, sizeof(wave));

        header format_header{};
        std::string("fmt ").copy(format_header.id, sizeof(format_header.id));

        format_chunk format_chunk{};
        format_chunk.format = static_cast<uint16_t>(format);
        format_chunk.channels = static_cast<uint16_t>(signal.channels());
        format_chunk.sample_rate = static_cast<uint32_t>(signal.sample_rate());

        std::vector<std::byte> format_ext_buffer;

        switch (format)
        {
            case wave_format::PCM:
            {
                format_ext_pcm format_ext{};
                format_ext_buffer.resize(sizeof(format_ext));

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

                format_chunk.byte_rate = format_chunk.sample_rate * format_ext.bits_per_sample * format_chunk.channels / CHAR_BIT;
                format_chunk.block_align = format_ext.bits_per_sample * format_chunk.channels / CHAR_BIT;
                std::memcpy(format_ext_buffer.data(), &format_ext, format_ext_buffer.size());
                break;
            }
            case wave_format::IEEE_FLOAT:
            {
                format_ext_ieee_float format_ext{};
                format_ext_buffer.resize(sizeof(format_ext));

                switch (data_type)
                {
                    case wave_data_type::FLOAT:
                    {
                        format_ext.bits_per_sample = 32;
                        break;
                    }
                    case wave_data_type::DOUBLE:
                    {
                        format_ext.bits_per_sample = 64;
                        break;
                    }
                    default:
                    {
                        //TODO: error
                        throw;
                        break;
                    }
                }

                format_chunk.byte_rate = format_chunk.sample_rate * format_ext.bits_per_sample * format_chunk.channels / CHAR_BIT;
                format_chunk.block_align = format_ext.bits_per_sample * format_chunk.channels / CHAR_BIT;
                std::memcpy(format_ext_buffer.data(), &format_ext, format_ext_buffer.size());
                break;
            }
            default:
            {
                std::stringstream format_stream{};
                format_stream << std::hex << static_cast<size_t>(format);
                throw std::runtime_error("Invalid format '" + format_stream.str() + "' for wave_file '" + m_path.string() + "'");
            }
        }

        format_header.size = static_cast<uint32_t>(sizeof(format_chunk) + format_ext_buffer.size());

        header data_header{};
        std::string("data").copy(data_header.id, sizeof(data_header.id));
        data_header.size = static_cast<uint32_t>(signal.size() * format_chunk.block_align);

        riff_header.size = static_cast<uint32_t>(sizeof(wave)
            + sizeof(format_header) + format_header.size
            + sizeof(data_header) + data_header.size
        );

        std::ofstream file(m_path, std::ios::binary | std::ios::trunc);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open wave_file '" + m_path.string() + "' for writing");
        }

        file.write(reinterpret_cast<const char*>(&riff_header), sizeof(riff_header));
        file.write(wave, sizeof(wave));
        file.write(reinterpret_cast<const char*>(&format_header), sizeof(format_header));
        file.write(reinterpret_cast<const char*>(&format_chunk), sizeof(format_chunk));
        file.write(reinterpret_cast<const char*>(format_ext_buffer.data()), format_ext_buffer.size());
        file.write(reinterpret_cast<const char*>(&data_header), sizeof(data_header));

        std::for_each(signal.begin(), signal.end(), [this, &data_type, &file](auto& samples) {
            std::for_each(samples.begin(), samples.end(), [this, &data_type, &file](auto& sample) {

                //TODO: support other formats
                switch (data_type)
                {
                    case wave_data_type::UINT8:
                    {
                        constexpr auto scale = (static_cast<size_t>(std::numeric_limits<uint8_t>::max()) + 1) / 2;
                        const uint8_t value = static_cast<uint8_t>(std::clamp<double>(sample * scale + scale, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()));
                        file.write(reinterpret_cast<const char*>(&value), sizeof(value));
                        break;
                    }
                    case wave_data_type::INT16:
                    {
                        constexpr auto scale = static_cast<size_t>(std::numeric_limits<int16_t>::max()) + 1;
                        const int16_t value = static_cast<int16_t>(std::clamp<double>(sample * scale, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max()));
                        file.write(reinterpret_cast<const char*>(&value), sizeof(value));
                        break;
                    }
                    case wave_data_type::INT24:
                    {
                        // No built-in type for 24 bit data
                        constexpr auto int24_min = -0x800000;
                        constexpr auto int24_max = 0x7FFFFF;
                        constexpr auto uint24_max = 0x1000000;
                        constexpr auto scale = int24_max + 1;

                        // Store in an int32 because there is no built in 24 bit type
                        int32_t value = static_cast<int32_t>(std::clamp<double>(sample * scale, int24_min, int24_max));

                        // Get 24 bit two's complement value from a 32 bit integer
                        if (value < 0)
                        {
                            value += uint24_max;
                        }

                        // Only write 3 bytes because it is a 24 bit value
                        file.write(reinterpret_cast<char*>(&value), 3);
                        break;
                    }
                    case wave_data_type::INT32:
                    {
                        constexpr auto scale = static_cast<size_t>(std::numeric_limits<int32_t>::max()) + 1;
                        const int32_t value = static_cast<int32_t>(std::clamp<double>(sample * scale, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max()));
                        file.write(reinterpret_cast<const char*>(&value), sizeof(value));
                        break;
                    }
                    case wave_data_type::FLOAT:
                    {
                        float value = static_cast<float>(sample);
                        file.write(reinterpret_cast<char*>(&value), sizeof(value));
                        break;
                    }
                    case wave_data_type::DOUBLE:
                    {
                        double value = static_cast<double>(sample);
                        file.write(reinterpret_cast<char*>(&value), sizeof(value));
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

        // Close the file for writing so we can read it to initialize member data
        file.close();

        // Initialize to fill in member data
        this->initialize();
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

        if (!m_initialized)
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