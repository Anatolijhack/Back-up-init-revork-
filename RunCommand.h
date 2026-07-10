#pragma once
#include "IComand.h"
class RunCommand : public ICommand
{
public:
	void execute(const std::vector<std::string>& args) override;
};