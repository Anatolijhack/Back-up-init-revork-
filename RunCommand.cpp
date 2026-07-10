#include "RunCommand.h"
#include "BackupManagger.h"
#include "Config.h"

void RunCommand::execute(const std::vector<std::string>& args) {
    Config cfg;

    for (auto& a : args) {
        if (a.find("--source=") == 0)
            cfg.sources = a.substr(9);
        else if (a.find("--dest=") == 0)
            cfg.destiantion = a.substr(7);
        else if (a == "--incremental")
            cfg.incremental = true;
        else if (a == "--sync")
            cfg.sync = true;
        else if (a == "--dry-run")
            cfg.dryRun = true;
    }  
    if (cfg.sources.empty() || cfg.destiantion.empty()) {
        throw std::runtime_error("source and destination must be specified");
    }
    BackupManager manager(cfg);
    manager.run();
}