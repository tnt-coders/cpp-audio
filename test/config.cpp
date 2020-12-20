#include "config.hpp"

#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>
#include <utility>

std::unordered_map<std::string, std::string> read_config(const std::filesystem::path& path)
{
    std::ifstream config_file{path};
    if (!config_file.is_open())
    {
        // TODO: error;
        throw;
    }

    std::unordered_map<std::string, std::string> config{};

    std::regex regex{"([^:]+):([^:]+)"};
    for (std::string line; std::getline(config_file, line);)
    {
        std::smatch match;
        if (std::regex_match(line, match, regex))
        {
            config.emplace(std::make_pair(match[1], match[2]));
        }
        else
        {
            // TODO: error
            throw;
        }
    }

    // TODO: implement
    return config;
}
