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
	using PathBatch = std::vector<std::filesystem::path>;
	using JobBatch = std::vector<FileJob>;
	BackupManager(const Config& cfg);
	void run();
	
private:
	void scanFiles();
	void removeExtraFiles();
	void builPipeline();

	Config config;
	ThreadPool pool;
	Logger logger;
	std::shared_ptr<Stage<PathBatch, PathBatch>> filterStage;
	std::shared_ptr<Stage<PathBatch, JobBatch>> prepareStage;
	std::shared_ptr<Stage<JobBatch, PathBatch>> copyStage;
	std::shared_ptr<Stage<PathBatch, PathBatch>> logStage;
	std::filesystem::path finalDest;
	size_t current_memory = 0;
	const size_t MAX_MEMORY = 50 * 1024 * 1024; // 50 MB
	std::mutex mem_mtx;
	std::condition_variable mem_cv;

};
