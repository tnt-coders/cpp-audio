#pragma once

#include "file.hpp"
#include "wave_file.hpp"
#include <filesystem>
#include <memory>

namespace tnt::audio
{

template <typename T>
std::unique_ptr<file<T>> make_file(const std::filesystem::path& path)
{
    // Try based on extension
    if (path.extension() == ".wav")
    {
        return std::make_unique<wave_file<T>>(path);
    }

    //TODO: invalid file
    throw;
}

} /* namespace tnt::audio */