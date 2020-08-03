#ifndef AIO_PREPROCESS_HPP
#define AIO_PREPROCESS_HPP

#include <AIO/Game/Board.hpp>

#include <vector>

namespace AIO::Network
{
using Tensor = std::vector<float>;

constexpr std::size_t PAST_MOVES = 4;
constexpr std::size_t TENSOR_DIM = (PAST_MOVES * 2) + 1;

Tensor StateToTensor(const Game::Board& state);
}  // namespace AIO::Network

#endif  // AIO_PREPROCESS_HPP
