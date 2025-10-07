#pragma once
#include <vector>
#include <string>
#include <filesystem>

// 指定フォルダの指定拡張子ファイルの一覧を取得
std::vector<std::string> SearchFiles(const std::filesystem::path& directory, const std::string& extension);

// 指定フォルダ以下の指定拡張子ファイルの相対パス一覧を取得
std::vector<std::string> SerchFilesPathsAddChild(const std::filesystem::path& directory, const std::string& extension);

std::wstring ConvertString(const std::string& str);

std::string ConvertString(const std::wstring& str);
