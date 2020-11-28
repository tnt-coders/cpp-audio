#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

std::unordered_map<std::string, std::string> read_config(const std::filesystem::path& path);