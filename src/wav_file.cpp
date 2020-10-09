#include <cstdint>

namespace tnt::audio
{

struct Header
{
    char riff[4];
    uint32_t riffSize;
    char wave[4];
    char fmt[4];
    uint32_t fmtSize;
    uint16_t format;
    uint16_t channels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
};

struct Chunk
{
    char id[4];
    uint32_t size;
};

} /* namespace tnt::audio */