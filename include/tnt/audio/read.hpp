#include "multisignal.hpp"
#include "reader.hpp"
#include "reader/wav_reader.hpp"
#include <memory>
#include <filesystem>

namespace tnt::audio
{

template <typename T>
multisignal<T> read(const std::filesystem::path& path)
{
    const auto reader = make_reader<T>(path);
    auto buffer = reader->buffer(reader->size());
    reader->read(&buffer);
    return buffer;
}

template <typename T>
std::unique_ptr<reader<T>> make_reader(const std::filesystem::path& path)
{
    // Try based on extension
    if (path.extension() == ".wav")
    {
        return std::make_unique<wav_reader<T>>(path);
    }

    // Try through brute force if no extensions match
    try
    {
        return std::make_unique<wav_reader<T>>(path);
    }
    catch (...)
    {
        // Failed (no warning necessary)
    }

    //TODO: error
    throw;
}


} /* namespace tnt::audio */