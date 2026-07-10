#pragma once
#include "IComand.h"

class StatusCommand : public ICommand {
public:
    void execute(const std::vector<std::string>& args) override;
};