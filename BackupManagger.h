#pragma once
#include "Config.h"
#include "looger.h"
#include <memory>
#include "ThreadPoolS.h"
struct FileJob
{
	std::filesystem::path src;
	std::filesystem::path dst;
};
class BackupManager
{
public:
	BackupManager(const Config& cfg);
	void run();
	
private:
	void scanFiles();
	void removeExtraFiles();
	void builPipeline();

	Config config;
	ThreadPool pool;
	Logger logger;
	std::shared_ptr<Stage<std::filesystem::path, std::filesystem::path>> filterStage;

	std::shared_ptr<Stage<std::filesystem::path, FileJob>> prepareStage;

	std::shared_ptr<Stage<FileJob, std::filesystem::path>> copyStage;

	std::shared_ptr<Stage<std::filesystem::path, std::filesystem::path>> logStage;
	std::filesystem::path finalDest;
};