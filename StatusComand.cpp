#include "StatusComand.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

void StatusCommand::execute(const std::vector<std::string>& args) {
    if (fs::exists("backup.log")) {
        std::cout << "Log file exists: backup.log\n";
    }
    else {
        std::cout << "No backup log found\n";
    }
}