#ifndef AIO_MATCH_OPTIONS_HPP
#define AIO_MATCH_OPTIONS_HPP

#include <AIO/Search/SearchOptions.hpp>

struct MatchOptions final
{
    AIO::Search::SearchOptions PlayerOpt1;
    AIO::Search::SearchOptions PlayerOpt2;

    int GameThreads{ 1 };
    int Games{ 1 };
    std::string DataDir;

    void Load(const std::string& filename);
};

#endif  // AIO_MATCH_OPTIONS_HPP
