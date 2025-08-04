#pragma once
#include <iostream>
#include <fstream>
#include <format>
#include <vector>

class Logger {
public:
	//ログファイルを作成する
	Logger();
	//ログファイルを登録する
	void RegistLogFile(std::string& logName);
	//ログを出力する
	void Log(const std::string &message);

	void operator<<(const std::string& message);
	void operator<<(const std::wstring& message);

private:

	int logStreamHandle = 0;

	static std::vector<std::ofstream> logStreams; //ログファイル一覧

};

std::wstring ConvertString(const std::string &str);

std::string ConvertString(const std::wstring &str);
