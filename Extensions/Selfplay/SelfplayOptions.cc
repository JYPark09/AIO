#include "SelfplayOptions.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

void SelfplayOptions::Load(const std::string& filename)
{
    SearchOpt.Load(filename);

    nlohmann::json j;
    std::ifstream(filename) >> j;

    GameThreads = j.value<int>("GameThreads", GameThreads);
    DataDir = j["DataDir"].get<std::string>();
}
