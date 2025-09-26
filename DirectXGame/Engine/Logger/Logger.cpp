#include "Logger.h"
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

void Logger::RegistLogFile(std::string logName) {
    //既に存在する場合はそのハンドルを返す
    for(int i = 0; i < logStreamNames.size(); ++i) {
        if (logStreamNames[i] == logName) {
            logStreamHandle = i; 
            return;
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
    
    //logsディレクトリが存在するか確認
    if (!fs::exists(path)) {
        fs::create_directory(path); // 存在しない場合は作成
    }

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

	logStreamHandle = static_cast<int>(logStreams.size() - 1);
}

void Logger::Log(const std::string &message) {
	//ログファイルに出力
	this->logStreams[logStreamHandle] << message << std::endl;
}

