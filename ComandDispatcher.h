#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "IComand.h"

class CommandDispatcher
{
public:
	void registerCommand(const std::string& name, std::unique_ptr<ICommand> cmd);
	void run(int argc, char* argv[]);
private:
	std::unordered_map < std::string, std::unique_ptr<ICommand>> commands;
};