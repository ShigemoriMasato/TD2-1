#pragma once
#include <iostream>
#include <fstream>
#include <format>
#include <vector>

class Logger {
public:
	//初期設定はmaster.log
	Logger();
	//ログファイルを登録する
	int RegistLogFile(std::string logName);
	//ログを出力する
	void Log(const std::string &message);

	void SetLogStreamHandle(int handle);

	void SetLogStreamName(std::string& logName);

	void operator<<(const std::string& message);
	void operator<<(const std::wstring& message);

private:

	int logStreamHandle = 0;

	static std::vector<std::ofstream> logStreams; //ログファイル一覧
	static std::vector<std::string> logStreamNames; //ログファイル名一覧

};

std::wstring ConvertString(const std::string &str);

std::string ConvertString(const std::wstring &str);
