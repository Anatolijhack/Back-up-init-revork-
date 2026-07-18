#include "BackupManagger.h"
#include "TimeUntils.h"
#include "FileHash.h"
#include <filesystem>
#include <unordered_set>
#include <map>


namespace fs = std::filesystem;

BackupManager::BackupManager(const Config& cfg)
    : config(cfg), pool(cfg.threads)
{
    finalDest = config.destiantion /
        (config.sources.filename().string() + "_" + getTimerstamp());
}

//void BackupManager::run() {
//    logger.log(LogLevel::INFO, "Backup started");
//
//    fs::create_directories(finalDest);
//    scanFiles();              
//    pool.waitFinished();
//    if (config.sync)
//    {
//        removeExtraFiles();
//    }
//
//    logger.log(LogLevel::INFO, "Backup finished");
//}
//void BackupManager::scanFiles() {
//    for (auto& entry : fs::recursive_directory_iterator(config.sources)) {
//        if (fs::is_regular_file(entry)) {
//            pool.enqueue([this, path = entry.path()]() {
//                processFile(path);
//                });
//        }
//    }
//}
//
//void BackupManager::processFile(const fs::path& file) {
//    fs::path relative = fs::relative(file, config.sources);
//    fs::path target = finalDest / relative;
//
//    fs::create_directories(target.parent_path());
//
//    if (!config.incremental || isModified(file, target))
//    {
//        fs::copy_file(file, target, fs::copy_options::overwrite_existing);
//        logger.log(LogLevel::INFO, "Copied: " + file.string());
//    }
//}
//void BackupManager::builPipeline()
//{
//    using namespace std::filesystem;
//
//    // Stage 1: filter files
//    filterStage = std::make_shared<Stage<path, path>>(pool,
//        [](path p) {
//            if (is_regular_file(p))
//                return p;
//            throw std::runtime_error("skip");
//        });
//
//    // Stage 2: prepare paths
//    prepareStage = std::make_shared<Stage<path, FileJob>>(pool,
//        [this](path file) {
//            path rel = relative(file, config.sources);
//            return FileJob{ file, finalDest / rel };
//        });
//
//    // Stage 3: copy
//    copyStage = std::make_shared<Stage<FileJob, path>>(pool,
//        [this](FileJob job) {
//            std::filesystem::create_directories(job.dst.parent_path());
//
//            std::filesystem::copy_file(
//                job.src,
//                job.dst,
//                std::filesystem::copy_options::overwrite_existing
//            );
//
//            logger.log(LogLevel::INFO, "Copied: " + job.src.string());
//
//            return job.dst;
//        });
//
//    // Stage 4: log result
//    logStage = std::make_shared<Stage<path, path>>(pool,
//        [this](path p) {
//            logger.log(LogLevel::INFO, "Done: " + p.string());
//            return p;
//        });
//
//    // соединяем pipeline
//    connect(filterStage, prepareStage);
//    connect(prepareStage, copyStage);
//    connect(copyStage, logStage);
//}
void BackupManager::builPipeline()
{
    using namespace std::filesystem;

    // Stage 1: filter
    filterStage = std::make_shared<Stage<PathBatch, PathBatch>>(pool,
        [](PathBatch batch) {
            PathBatch out;
            out.reserve(batch.size());

            for (auto& p : batch)
            {
                if (is_regular_file(p))
                    out.push_back(p);
            }
            return out;
        });

    // Stage 2: prepare
    prepareStage = std::make_shared<Stage<PathBatch, JobBatch>>(pool,
        [this](PathBatch batch) {
            JobBatch jobs;
            jobs.reserve(batch.size());

            for (auto& file : batch)
            {
                auto rel = std::filesystem::relative(file, config.sources);
                jobs.push_back({ file, finalDest / rel });
            }
            return jobs;
        });

    // Stage 3: copy
    copyStage = std::make_shared<Stage<JobBatch, PathBatch>>(pool,
        [this](JobBatch jobs) {
            PathBatch done;
            done.reserve(jobs.size());

            for (auto& job : jobs)
            {
                std::filesystem::create_directories(job.dst.parent_path());

                std::filesystem::copy_file(
                    job.src,
                    job.dst,
                    std::filesystem::copy_options::overwrite_existing
                );

                done.push_back(job.dst);
            }
            return done;
        });

    // Stage 4: log
    logStage = std::make_shared<Stage<PathBatch, PathBatch>>(pool,
        [this](PathBatch batch) {

            for (auto& p : batch)
            {
                logger.log(LogLevel::INFO, "Done: " + p.string());
            }

            size_t mem = batch.size() * sizeof(std::filesystem::path);

            {
                std::lock_guard<std::mutex> lock(mem_mtx);
                current_memory -= mem;
            }

            mem_cv.notify_all(); // 🔥 разбудить scanFiles

            return batch;
        });

    connect(filterStage, prepareStage);
    connect(prepareStage, copyStage);
    connect(copyStage, logStage);
}
void BackupManager::run()
{
    logger.log(LogLevel::INFO, "Backup started");

    std::filesystem::create_directories(finalDest);

    builPipeline();   // 🔥 важно

    scanFiles();       // пушим в pipeline

    pool.wait();       // ждём ВСЁ

    if (config.sync)
    {
        removeExtraFiles();
    }

    logger.log(LogLevel::INFO, "Backup finished");
}
void BackupManager::scanFiles()
{
    using namespace std::filesystem;

    const size_t BATCH_SIZE = 200;

    PathBatch batch;
    batch.reserve(BATCH_SIZE);

    for (auto& entry : recursive_directory_iterator(
        config.sources,
        directory_options::skip_permission_denied))
    {
        batch.push_back(entry.path());

        if (batch.size() >= BATCH_SIZE)
        {
            size_t mem = batch.size() * sizeof(path);

            // 🔥 ЖДЁМ ЕСЛИ ПАМЯТЬ ПЕРЕПОЛНЕНА
            {
                std::unique_lock<std::mutex> lock(mem_mtx);
                mem_cv.wait(lock, [this, mem]() {
                    return current_memory + mem < MAX_MEMORY;
                    });

                current_memory += mem;
            }

            filterStage->push(std::move(batch));
            batch.clear();
        }
    }

    if (!batch.empty())
    {
        size_t mem = batch.size() * sizeof(path);

        {
            std::unique_lock<std::mutex> lock(mem_mtx);
            mem_cv.wait(lock, [this, mem]() {
                return current_memory + mem < MAX_MEMORY;
                });

            current_memory += mem;
        }

        filterStage->push(std::move(batch));
    }
}
//void BackupManager::scanFiles()
//{
//    using namespace std::filesystem;
//
//    for (auto& entry : recursive_directory_iterator(
//        config.sources,
//        directory_options::skip_permission_denied))
//    {
//        filterStage->push(entry.path());
//    }
//}
void BackupManager::removeExtraFiles()
{
    for (auto& entry : fs::recursive_directory_iterator(finalDest))
    {
        if (!fs::is_regular_file(entry))
        {
            continue;
        }
        fs::path relative = fs::relative(entry.path(), finalDest);
        fs::path srcFile = config.sources / relative;

        if (!fs::exists(srcFile))
        {
            try
            {
                if (config.dryRun)
                {
                    
                    logger.log(LogLevel::INFO, "[DRY RUN] Would delete:" + entry.path().string());
                }
                else
                {
                    
                    fs::remove(entry.path());
                    logger.log(LogLevel::WARM, "Deleted:" + entry.path().string());
                }
            }  
            catch (...)
            {
                
                logger.log(LogLevel::ERROR, "Failed to open file:" + entry.path().string());
            }
        }
    }
}
