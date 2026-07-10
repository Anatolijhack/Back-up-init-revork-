#pragma once
#include <filesystem>
namespace fs = std::filesystem;
struct Config
{
	fs::path sources;
	fs::path destiantion;
	int threads = 4;
	bool incremental = false;
	bool sync = false;
	bool dryRun = false;
};