#include "SelfplayGame.hpp"
#include "SelfplayOptions.hpp"

#include <atomic>
#include <csignal>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <numeric>
#include <sstream>
#include <thread>
#include <vector>

#include <spdlog/spdlog.h>

using namespace AIO;

namespace fs = std::filesystem;

std::atomic<bool> running{ true };
void signalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
    {
        spdlog::info("received stop signal");
        running = false;
    }
}

std::string makeDateStr()
{
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);

    std::stringstream ss;

    ss << (now->tm_year + 1900) << '_' << (now->tm_mon + 1) << '_'
       << now->tm_mday << '/' << now->tm_hour << '_' << now->tm_min << '_'
       << now->tm_sec;

    return ss.str();
}

int main()
{
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    SelfplayOptions opt;
    opt.Load("selfplay.json");

    std::vector<std::thread> gameThreads(opt.GameThreads);

    if (fs::exists(opt.DataDir))
        fs::create_directories(opt.DataDir);

    spdlog::info("start {} game threads", opt.GameThreads);

    for (int threadId = 0; threadId < opt.GameThreads; ++threadId)
    {
        gameThreads[threadId] = std::thread([&opt, threadId] {
            while (running.load())
            {
                const std::string dataDir = opt.DataDir + "/" + makeDateStr() +
                                            "_" + std::to_string(threadId);

                fs::create_directories(dataDir);
                const auto data = RunGame(opt);

                std::ofstream giboFile(dataDir + "/gibo");
                std::ofstream featFile(dataDir + "/feat");
                std::ofstream probFile(dataDir + "/prob");
                std::ofstream valFile(dataDir + "/val");

                for (const auto& td : data)
                {
                    if (std::accumulate(td.pi.begin(), td.pi.end(), 0) == 0)
                        continue;

                    giboFile << Game::PointUtil::PointStr(td.move);

                    auto featEndIt = td.state.end();
                    auto probEndIt = td.pi.end();
                    auto valEndIt = td.z;

                    for (auto it = td.state.begin(); it != featEndIt; ++it)
                        featFile << static_cast<int>(*it) << ' ';
                    featFile << '\n';

                    for (auto it = td.pi.begin(); it != probEndIt; ++it)
                        probFile << *it << ' ';
                    probFile << '\n';

                    valFile << td.z << '\n';
                }
            }
        });
    }

    for (auto& thread : gameThreads)
        if (thread.joinable())
            thread.join();

    spdlog::info("selfplay shutdown");
}
