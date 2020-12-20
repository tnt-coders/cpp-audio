#pragma once

#include <filesystem>
#include <string>
#include <tnt/audio/multisignal.hpp>
#include <tnt/dsp/signal_generator.hpp>
#include <unordered_map>

std::unordered_map<std::string, std::string> read_config(const std::filesystem::path& path);

// TODO: Make more flexible so different channels can have different signal types
template <typename T>
tnt::audio::Multisignal<T> signal_from_config(const std::filesystem::path& path)
{
    const auto config      = read_config(path);
    const auto type        = config.at("type");
    const auto sample_rate = std::stoull(config.at("sample_rate"));
    const auto frequency   = static_cast<T>(std::stod(config.at("frequency")));
    const auto channels    = std::stoull(config.at("channels"));
    const auto size        = std::stoull(config.at("size"));
    const auto g           = tnt::dsp::SignalGenerator<T>(sample_rate, size);

    tnt::audio::Multisignal<T> signal(sample_rate, size);
    if (type == "cos")
    {
        for (size_t c = 0; c < channels; ++c)
        {
            signal.add_channel(g.cosine(frequency));
        }
        return signal;
    }
    else if (type == "sin")
    {
        for (size_t c = 0; c < channels; ++c)
        {
            signal.add_channel(g.sine(frequency));
        }
        return signal;
    }
    else
    {
        throw std::runtime_error("Invalid config value for 'type' in" + path.string());
    }
}
