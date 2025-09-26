#include "MyString.h"
#define NOMINMAX
#include <Windows.h>

namespace fs = std::filesystem;

std::vector<std::string> SearchFilesWithExtension(const std::filesystem::path& directory, const std::string& extension) {
    std::vector<std::string> contents;

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        throw std::runtime_error("Invalid directory: " + directory.string());
    }

    for (const auto& entry : fs::directory_iterator(directory)) {
        std::string path = entry.path().string();
        path.erase(0, directory.string().length() + 1);
        contents.push_back(path);
    }

    return contents;
}

std::wstring ConvertString(const std::string& str) {
    if (str.empty()) {
        return std::wstring();
    }

    auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
    if (sizeNeeded == 0) {
        return std::wstring();
    }
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
    return result;
}

std::string ConvertString(const std::wstring& str) {
    if (str.empty()) {
        return std::string();
    }

    auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
    if (sizeNeeded == 0) {
        return std::string();
    }
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
    return result;
}
