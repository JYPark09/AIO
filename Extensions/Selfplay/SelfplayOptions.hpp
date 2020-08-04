#ifndef AIO_SELFPLAY_OPTIONS_HPP
#define AIO_SELFPLAY_OPTIONS_HPP

#include <AIO/Search/SearchOptions.hpp>

struct SelfplayOptions final
{
    AIO::Search::SearchOptions SearchOpt;

    int GameThreads{ 1 };
    std::string DataDir;

    void Load(const std::string& filename);
};

#endif  // AIO_SELFPLAY_OPTIONS_HPP
