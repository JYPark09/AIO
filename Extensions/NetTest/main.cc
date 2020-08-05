#include <AIO/Network/FakeNetwork.hpp>
#include <AIO/Network/Preprocess.hpp>
#include <AIO/Network/TrtNetwork.hpp>
#include <AIO/Search/SearchOptions.hpp>

#include <iomanip>
#include <iostream>

using namespace AIO;

int main(int argc, char** argv)
{
    // if (argc != 2)
    // {
    //     std::cerr << "usage: " << argv[0] << " engine\n";
    //     return 1;
    // }

    Game::Board board;

    const int BATCH_SIZE = 16;

    std::vector<Network::Tensor> inp(BATCH_SIZE);
    for (int i = 0; i < BATCH_SIZE; ++i)
        inp[i] = Network::StateToTensor(board);

#ifdef BUILD_WITH_TRT
    Search::SearchOptions opt;
    Network::TrtNetwork network(opt, 0);
#else
    Network::FakeNetwork network;
#endif

    network.Initialize(argv[1]);

    std::vector<Network::Tensor> policy(BATCH_SIZE);
    Network::Tensor value(BATCH_SIZE);

    network.Evaluate(inp, policy, value);

    for (int i = 1; i <= BATCH_SIZE; ++i)
    {
        std::cout << "Batch " << i << '\n';

        std::cout << "Policy\n";

        for (std::size_t y = 0; y < Game::BOARD_HEIGHT; ++y)
        {
            for (std::size_t x = 0; x < Game::BOARD_WIDTH; ++x)
            {
                std::cout
                    << std::setw(4) << std::setfill('0')
                    << static_cast<int>(
                           policy[i - 1][x + y * Game::BOARD_WIDTH] *
                           10000.f)
                    << ' ';
            }

            std::cout << '\n';
        }

        std::cout << "Value " << value[i - 1] << "\n\n";
    }
}
