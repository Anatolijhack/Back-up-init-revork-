#pragma once
#include "IComand.h"

class InitCommand : public ICommand {
public:
    void execute(const std::vector<std::string>& args) override;
};