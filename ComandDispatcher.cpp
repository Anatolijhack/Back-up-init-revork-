#include "ComandDispatcher.h"
#include <iostream>

void CommandDispatcher::registerCommand(const std::string& name, std::unique_ptr<ICommand> cmd)
{
	commands[name] = std::move(cmd);
}
void CommandDispatcher::run(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage:Backup Command\n";
		return;
	}
	std::string command = argv[1];
	std::vector<std::string> args;
	for (int i = 2; i < argc; i++)
	{
		args.push_back(argv[i]);
	}
	if (commands.find(command) != commands.end())
	{
		commands[command]->execute(args);
	}
	else
	{
		std::cout << "Unknown command" << command << "\n";
	}
}