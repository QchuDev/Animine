#ifndef PATHS_H
#define PATHS_H

#include <filesystem>
#include <string>

inline std::filesystem::path g_basePath;

inline std::string assetPath(const std::string& relative) {
    return (g_basePath / relative).string();
}

#endif
