#ifndef AIO_SELFPLAY_GAME_HPP
#define AIO_SELFPLAY_GAME_HPP

#include "SelfplayOptions.hpp"

#include <AIO/Network/Preprocess.hpp>

#include <vector>

struct TrainingData
{
    AIO::Game::Point move;
    AIO::Network::Tensor state;
    std::vector<int> pi;
    float z;
};

std::vector<TrainingData> RunGame(const SelfplayOptions& opt);

#endif  // AIO_SELFPLAY_GAME_HPP
