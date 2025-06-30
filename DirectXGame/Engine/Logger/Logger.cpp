#include "Logger.h"
#include <Windows.h>
#include <chrono>

Logger::Logger(std::string logName) {
	//現在時刻を取得
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	//コンマ以下を切り捨てる
	std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
		nowSec = std::chrono::time_point_cast<std::chrono::seconds>(now);
	//日本時間に変換
	std::chrono::zoned_time localTime{ std::chrono::current_zone(), nowSec };
	//formatを使って年月日_時分秒の文字列型に変換
	std::string dataString = std::format("{:%Y%m%d_%H.%M.%S}", localTime);
	//ファイル名の決定
	std::string logFileName = "Logs/" + logName + "/" + dataString + ".log";
	//ファイルを作成して書き込み準備
	logStream = std::ofstream(logFileName);

	Log("Process started\n");
}

void Logger::Log(const std::string &message) {
	//ログファイルに出力
	this->logStream << message << std::endl;
}


std::wstring ConvertString(const std::string &str) {
    if (str.empty()) {
        return std::wstring();
    }

    auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char *>(&str[0]), static_cast<int>(str.size()), NULL, 0);
    if (sizeNeeded == 0) {
        return std::wstring();
    }
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char *>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
    return result;
}

std::string ConvertString(const std::wstring &str) {
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