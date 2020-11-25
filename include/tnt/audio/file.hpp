#pragma once

#include "file_base.hpp"
#include "impl/wav_file.hpp"
#include <filesystem>
#include <memory>

namespace tnt::audio
{

template <typename T>
std::unique_ptr<file_base<T>> file(const std::filesystem::path& path)
{
    // Try based on extension
    if (path.extension() == ".wav")
    {
        return std::make_unique<impl::wav_file<T>>(path);
    }

    //TODO: invalid file
    throw;
}

} /* namespace tnt::audio */