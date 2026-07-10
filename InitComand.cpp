#include "InitCommand.h"
#include <iostream>
#include <fstream>

void InitCommand::execute(const std::vector<std::string>& args) {
    std::ofstream cfg("backup.conf");

    cfg << "source=\n";
    cfg << "destination=\n";
    cfg << "threads=4\n";
    cfg << "incremental=false\n";
    cfg << "sync=false";

    std::cout << "Config file created: backup.conf\n";
}