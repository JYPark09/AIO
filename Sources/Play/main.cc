#include <AIO/Search/SearchEngine.hpp>

#include <iostream>

using namespace AIO;

int main()
{
    Search::SearchOptions option;
    option.NumEvalThreads = 10;
    option.NumSearchThreads = 10;

    Search::SearchEngine engine(option);

    std::cin.get();
}
