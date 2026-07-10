#include "looger.h"


Logger::Logger(const std::string& filename)
    : file(filename, std::ios::app) {}

void Logger::log(LogLevel level, const std::string& msg)
{
    std::lock_guard<std::mutex> lock(mtx);
    file << "[" << levelToString(level) << "] " << msg << "\n";
}

std::string Logger::levelToString(LogLevel lvl) {
    switch (lvl) {
    case LogLevel::INFO: return "INFO";
    case LogLevel::WARM: return "WARN";
    case LogLevel::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}