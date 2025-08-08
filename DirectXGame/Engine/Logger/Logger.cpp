#include "Logger.h"
#include <Windows.h>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

std::vector<std::ofstream> Logger::logStreams;
std::vector<std::string> Logger::logStreamNames;

Logger::Logger() {
    std::string a = "master";
    if (logStreams.size() == 0) {
        RegistLogFile(a);
    }

    logStreamHandle = 0;
}

int Logger::RegistLogFile(std::string logName) {
    for(int i = 0; i < logStreamNames.size(); ++i) {
        if (logStreamNames[i] == logName) {
            return i; // 既に存在する場合はそのハンドルを返す
        }
	}

#pragma region ファイル名を作成
    //現在時刻を取得
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    //コンマ以下を切り捨てる
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
        nowSec = std::chrono::time_point_cast<std::chrono::seconds>(now);
    //日本時間に変換
    std::chrono::zoned_time localTime{ std::chrono::current_zone(), nowSec };
    //formatを使って年月日_時分秒の文字列型に変換
    std::string dataString = std::format("{:%Y%m%d_%H.%M.%S}", localTime);
#pragma endregion

#pragma region ディレクトリ検索(作成)
    std::string path = "Logs";  // 検索対象のディレクトリ
    
    bool isFind = false;
    for (const auto& entry : fs::directory_iterator(path)) {
        std::cout << entry.path() << std::endl;
        if (entry.path() == logName) {
            isFind = true;
            break;
        }
    }

	//ディレクトリが見つからなければ作成
    if (!isFind) {
		fs::create_directories("Logs/" + logName);
    }

#pragma endregion

    //ファイル名の決定
    std::string logFileName = "Logs/" + logName + "/" + dataString + ".log";
    //ファイルを作成して書き込み準備
    logStreams.push_back(std::ofstream(logFileName));

	logStreamNames.push_back(logName);

	return int(logStreams.size() - 1);
}

void Logger::Log(const std::string &message) {
	//ログファイルに出力
	this->logStreams[logStreamHandle] << message << std::endl;
}

void Logger::SetLogStreamHandle(int handle) {
    if (handle < 0 || handle >= logStreams.size()) {
        throw std::out_of_range("Invalid log stream handle.");
    }
	logStreamHandle = handle;
}

void Logger::SetLogStreamName(std::string& logName) {
    for (int i = 0; i < logStreamNames.size(); ++i) {
        if(logStreamNames[i] == logName) {
            logStreamHandle = i;
            return;
		}
    }
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

void Logger::operator<<(const std::string& message) {
    this->logStreams[logStreamHandle] << message << std::endl;
}

void Logger::operator<<(const std::wstring& message) {
    this->logStreams[logStreamHandle] << ConvertString(message) << std::endl;
}
