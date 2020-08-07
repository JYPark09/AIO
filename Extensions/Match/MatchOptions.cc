#include "MatchOptions.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

void MatchOptions::Load(const std::string& filename)
{
    nlohmann::json j;
    std::ifstream(filename) >> j;

    const std::string player1 = j["Player1Cfg"].get<std::string>();
    const std::string player2 = j["Player2Cfg"].get<std::string>();

    PlayerOpt1.Load(player1);
    PlayerOpt2.Load(player2);

    GameThreads = j.value<int>("GameThreads", GameThreads);
    Games = j.value<int>("Games", Games);
    DataDir = j["DataDir"].get<std::string>();
}
