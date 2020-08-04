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
    WeightFileName = j["WeightFileName"].get<std::string>();
    Gpus = j.value<std::vector<int>>("Gpus", Gpus);

    NumEvalThreads = j.value<int>("NumEvalThreads", NumEvalThreads);
    NumSearchThreads = j.value<int>("NumSearchThreads", NumSearchThreads);
    BatchSize = j.value<int>("BatchSize", BatchSize);

    VirtualLoss = j.value<float>("VirtualLoss", VirtualLoss);
    cPUCT = j.value<float>("cPUCT", cPUCT);

    EnableDirichletNoise =
        j.value<bool>("EnableDirichletNoise", EnableDirichletNoise);
    DirichletNoiseAlpha =
        j.value<float>("DirichletNoiseAlpha", DirichletNoiseAlpha);
    DirichletNoiseEps = j.value<float>("DirichletNoiseEps", DirichletNoiseEps);

    MaxSimulations = j.value<int>("MaxSimulations", MaxSimulations);

    Verbose = j.value<bool>("Verbose", Verbose);
}
}  // namespace AIO::Search
