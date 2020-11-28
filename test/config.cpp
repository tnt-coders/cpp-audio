#include "config.hpp"
#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>

#include <iostream>

std::unordered_map<std::string, std::string> read_config(const std::filesystem::path& path)
{
    std::ifstream config_file{ path };
    if (!config_file.is_open())
    {
        //TODO: error;
        throw;
    }

    std::regex regex{ "([^:]+):([^:]+)" };
    for (std::string line; std::getline(config_file, line);)
    {
        std::smatch match;
        if (std::regex_match(line, match, regex))
        {
            std::cout << match[1] << ":" << match[2] << std::endl;
        }
        else
        {
            //TODO: error
            throw;
        }
    }

    //TODO: implement
    return std::unordered_map<std::string, std::string>{};
}