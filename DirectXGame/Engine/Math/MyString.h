#pragma once
#include <vector>
#include <string>
#include <filesystem>

//どっかに置きたいけど場所が思いつかないのでとりあえず匿名名前空間に置いとく。多分ConvertStringとかと一緒にどっか新しく作る
std::vector<std::string> SearchFilesWithExtension(const std::filesystem::path& directory, const std::string& extension);

std::wstring ConvertString(const std::string& str);

std::string ConvertString(const std::wstring& str);
