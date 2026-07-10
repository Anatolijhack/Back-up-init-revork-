#include <iostream>
#include <memory>
#include "ComandDispatcher.h"
#include "RunCommand.h"
#include "InitCommand.h"
#include "StatusComand.h"
#include "BackupManagger.h"

//int main(int argc, char* argv[]) {
//    CommandDispatcher dispatcher;
//
//    // Регистрируем команды
//    dispatcher.registerCommand("run", std::make_unique<RunCommand>());
//    dispatcher.registerCommand("init", std::make_unique<InitCommand>());
//    dispatcher.registerCommand("status", std::make_unique<StatusCommand>());
//
//    try {
//        dispatcher.run(argc, argv);
//    }
//    catch (const std::exception& e) {
//        std::cerr << "Fatal error: " << e.what() << "\n";
//        return 1;
//    }
//    catch (...) {
//        std::cerr << "Unknown fatal error\n";
//        return 1;
//    }
//
//    return 0;
//}
int main()
{
    try
    {
        Config cfg;
        cfg.sources = "C:\\Users\\Kirill\\Downloads"; // укажи свою папку
        cfg.destiantion = "Test"; // куда копировать
        cfg.incremental = false;
        cfg.sync = false;
        cfg.dryRun = false;
        cfg.threads = 4;

        std::cout << "Starting backup...\n";

        BackupManager manager(cfg);
        manager.run();

        std::cout << "Backup finished successfully\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}