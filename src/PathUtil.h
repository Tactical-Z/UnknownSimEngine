#pragma once

#include <filesystem>
#include <string>

// For removing intellisense, define comes from cmake.
#define PROJECT_SOURCE_DIR

namespace PathUtil
{
    inline std::string project_dir(const std::string& path)
    {
        return (std::filesystem::path(PROJECT_SOURCE_DIR) / path).string();
    }

    inline std::string asset_dir(const std::string& path)
    {
        return (std::filesystem::path(ASSET_DIR) / path).string();
    }

    inline std::string shader_dir(const std::string& path)
    {
        return (std::filesystem::path(SHADER_DIR) / path).string();
    }

    inline std::string src_dir(const std::string& path)
    {
        return (std::filesystem::path(SRC_DIR) / path).string();
    }
}