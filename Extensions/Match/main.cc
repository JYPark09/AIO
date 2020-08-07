#include "MatchGame.hpp"
#include "MatchOptions.hpp"

#include <atomic>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

#include <spdlog/spdlog.h>

using namespace AIO;
namespace fs = std::filesystem;

using std::cerr;
using std::cout;
using std::endl;

std::string makeDateStr()
{
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);

    std::stringstream ss;

    ss << (now->tm_year + 1900) << '_' << (now->tm_mon + 1) << '_'
       << now->tm_mday << '_' << now->tm_hour << '_' << now->tm_min << '_'
       << now->tm_sec;

    return ss.str();
}

int main(int argc, char** argv)
{
    /*if (argc != 2)
    {
        cerr << "usage: " << argv[0] << " <option file>\n";
        return 1;
    }*/

    MatchOptions opt;
    //opt.Load(argv[1]);
    opt.Load("match.json");

    std::vector<std::thread> gameThreads(opt.GameThreads);

    if (fs::exists(opt.DataDir))
        fs::create_directories(opt.DataDir);

    std::atomic<int> games = 0;

    std::atomic<int> p1BlackWins = 0;
    std::atomic<int> p2BlackWins = 0;
    std::atomic<int> p1WhiteWins = 0;
    std::atomic<int> p2WhiteWins = 0;
    std::atomic<int> draws = 0;

    spdlog::info("start {} game threads", opt.GameThreads);

    for (int threadId = 0; threadId < opt.GameThreads; ++threadId)
    {
        gameThreads[threadId] =
            std::thread([&opt, threadId, &games, &p1BlackWins, &p2BlackWins,
                         &p1WhiteWins, &p2WhiteWins, &draws] {
                bool p1IsBlack = threadId % 2;

                while (games < opt.Games)
                {
                    ++games;

                    std::string dataDir = opt.DataDir + "/" + makeDateStr() +
                                          '_' + std::to_string(threadId) + '_';

                    const auto data = RunGame(opt, p1IsBlack);

                    if (data.winner == 0)
                    {
                        ++draws;
                    }
                    else if (data.winner == 1)
                    {
                        if (data.winColor == Game::P_BLACK)
                            ++p1BlackWins;
                        else
                            ++p1WhiteWins;
                    }
                    else if (data.winner == 2)
                    {
                        if (data.winColor == Game::P_BLACK)
                            ++p2BlackWins;
                        else
                            ++p2WhiteWins;
                    }

                    dataDir +=
                        Game::ColorUtil::ColorStr(data.winColor) + "_gibo";
                    std::ofstream giboFile(dataDir);

                    for (const auto move : data.history)
                        giboFile << Game::PointUtil::PointStr(move);

                    p1IsBlack = !p1IsBlack;
                }
            });
    }

    spdlog::info("start to play {} games", opt.Games);

    for (auto& thread : gameThreads)
        if (thread.joinable())
            thread.join();

    std::ofstream resultFile(opt.DataDir + "/result");
    resultFile << "Total games: " << games.load() << '\n'
               << "Player1: " << opt.PlayerOpt1.WeightFileName << '\n'
               << "Player2: " << opt.PlayerOpt2.WeightFileName << '\n'
               << "=============================================\n"
               << "       player1    player2\n"
               << "black " << std::right << std::setw(7) << p1BlackWins
               << "    " << std::right << std::setw(7) << p2BlackWins << "    "
               << (p1BlackWins + p2BlackWins) << '\n'
               << "white " << std::right << std::setw(7) << p1WhiteWins
               << "    " << std::right << std::setw(7) << p2WhiteWins << "    "
               << (p1WhiteWins + p2WhiteWins) << '\n'
               << "      " << std::right << std::setw(7)
               << (p1BlackWins + p1WhiteWins) << "    " << std::right
               << std::setw(7) << (p2BlackWins + p2WhiteWins) << '\n'
               << "draws : " << draws << '\n';

    spdlog::info("match shutdown");
}
