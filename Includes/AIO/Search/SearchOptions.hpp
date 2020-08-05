#ifndef AIO_SEARCH_OPTIONS_HPP
#define AIO_SEARCH_OPTIONS_HPP

#include <string>
#include <vector>

namespace AIO::Search
{
struct SearchOptions final
{
    std::string WeightFileName{ "" };
    std::vector<int> Gpus;

    int NumEvalThreads{ 1 };
    int NumSearchThreads{ 1 };
    int BatchSize{ 1 };

    float VirtualLoss{ 0 };
    float cPUCT{ 1.8f };

    int MaxSimulations{ 100 };

    bool EnableDirichletNoise{ false };
    float DirichletNoiseAlpha{ 0.03f };
    float DirichletNoiseEps{ 0.75f };

    bool Verbose{ true };

    void Load(const std::string_view& filename);
};
}  // namespace AIO::Search

#endif  // AIO_SEARCH_OPTIONS_HPP
