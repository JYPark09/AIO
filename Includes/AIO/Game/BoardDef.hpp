#ifndef AIO_BOARD_DEF_HPP
#define AIO_BOARD_DEF_HPP

#include <cstddef>
#include <string>

namespace AIO::Game
{
constexpr std::size_t BOARD_WIDTH = 8;
constexpr std::size_t BOARD_HEIGHT = 8;
constexpr std::size_t BOARD_SIZE = BOARD_HEIGHT * BOARD_WIDTH;

using StoneColor = int;
constexpr StoneColor P_BLACK = -1;
constexpr StoneColor P_NONE = 0;
constexpr StoneColor P_WHITE = 1;
constexpr StoneColor P_INVALID = -100;

constexpr StoneColor Opponent(StoneColor color);
std::string ColorStr(StoneColor color);
}  // namespace AIO::Game

#endif  // AIO_BOARD_DEF_HPP
