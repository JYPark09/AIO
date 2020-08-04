#include <AIO/Search/SearchEngine.hpp>

#include <iostream>
#include <thread>

using namespace AIO;

int main()
{
    using namespace std::chrono_literals;

    Search::SearchOptions option;
    option.NumEvalThreads = 10;
    option.NumSearchThreads = 10;

    Search::SearchEngine engine(option);

    engine.Search();

    engine.DumpStats();
}
