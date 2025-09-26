#pragma once
#include <iostream>
#include <fstream>
#include <format>
#include <vector>
#include <Math/MyString.h>

class Logger {
public:
	//初期設定はmaster.log
	Logger();
	//ログファイルを登録する
	void RegistLogFile(std::string logName);
	//ログを出力する
	void Log(const std::string &message);
	
private:

	int logStreamHandle = 0;

	static std::vector<std::ofstream> logStreams; //ログファイル一覧
	static std::vector<std::string> logStreamNames; //ログファイル名一覧

};
