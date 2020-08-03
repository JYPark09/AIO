#ifndef AIO_SEARCH_OPTIONS_HPP
#define AIO_SEARCH_OPTIONS_HPP

#include <string_view>

namespace AIO::Search
{
struct SearchOptions final
{
    int NumSearchThreads{ 1 };

    float VirtualLoss{ 0 };
    float cPUCT{ 1.8f };

    int MaxSimulations{ 100 };

    void Load(const std::string_view& filename);
};
}  // namespace AIO::Search

#endif  // AIO_SEARCH_OPTIONS_HPP
