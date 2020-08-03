#include <AIO/Search/SearchOptions.hpp>

#include <fstream>
#include <nlohmann/json.hpp>

namespace AIO::Search
{
void SearchOptions::Load(const std::string_view& filename)
{
    std::ifstream file(filename.data());
    if (!file.is_open())
        return;

    nlohmann::json j;
    file >> j;

    // Fill data
    NumSearchThreads = j.value<int>("NumSearchThreads", NumSearchThreads);

    VirtualLoss = j.value<float>("VirtualLoss", VirtualLoss);
    cPUCT = j.value<float>("cPUCT", cPUCT);

    MaxSimulations = j.value<int>("MaxSimulations", MaxSimulations);
}
}  // namespace AIO::Search
