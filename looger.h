#pragma once
#include <fstream>
#include <mutex>
#include <iostream>
enum class LogLevel {INFO,WARM,ERROR};
class Logger
{
public:
	Logger(const std::string& fillename = "backup.log");
	void log(LogLevel level, const std::string& msg);

private:
	std::ofstream file;
	std::mutex mtx;

	std::string levelToString(LogLevel lvl);
};