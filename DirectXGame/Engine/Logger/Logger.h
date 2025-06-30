#pragma once
#include <iostream>
#include <fstream>
#include <format>

class Logger {
public:
	//ログファイルを作成する
	Logger(std::string logName);
	//ログを出力する
	void Log(const std::string &message);

private:

	std::ofstream logStream;		//ログファイルストリーム

};

std::wstring ConvertString(const std::string &str);

std::string ConvertString(const std::wstring &str);
