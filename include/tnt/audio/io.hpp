#pragma once

#include "file.hpp"
#include "wave_file.hpp"

#include <filesystem>
#include <memory>
#include <stdexcept>

namespace tnt::audio
{

/*!
\brief Creates a pointer to an audio file object

The type of audio file is determined by the extension of the file provided. For example, a ".wav"
file will result in a pointer to a WaveFile object being produced.

\param[in] path Path to the file
\return Unique pointer to an audio file object
*/
template <typename T>
std::unique_ptr<File<T>> file(const std::filesystem::path& path)
{
    // Check if the etension is valid
    if (path.extension() == ".wav")
    {
        return std::make_unique<WaveFile<T>>(path);
    }

    throw std::runtime_error("Unrecognized file extension for audio file '" + path.string() + "'");
}

}  // namespace tnt::audio
