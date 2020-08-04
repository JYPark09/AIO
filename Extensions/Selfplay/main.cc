#include "SelfplayGame.hpp"
#include "SelfplayOptions.hpp"

#include <ctime>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>

#include <spdlog/spdlog.h>

using namespace AIO;

namespace fs = std::filesystem;

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
    SelfplayOptions opt;
    opt.Load("selfplay.json");

    std::vector<std::thread> gameThreads(opt.GameThreads);

    if (fs::exists(opt.DataDir))
        fs::create_directories(opt.DataDir);

    spdlog::info("start {} game threads", opt.GameThreads);

    for (int threadId = 0; threadId < opt.GameThreads; ++threadId)
    {
        gameThreads[threadId] = std::thread([&opt, threadId] {
            while (true)
            {
                const std::string dataDir = opt.DataDir + "/" + makeDateStr() +
                                            "_" + std::to_string(threadId);

                fs::create_directories(dataDir);
                const auto data = RunGame(opt);

                std::ofstream featFile(dataDir + "/feat");
                std::ofstream probFile(dataDir + "/prob");
                std::ofstream valFile(dataDir + "/val");

                for (const auto& td : data)
                {
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
}
