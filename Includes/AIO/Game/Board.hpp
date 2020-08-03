#ifndef AIO_BOARD_HPP
#define AIO_BOARD_HPP

#include <AIO/Game/BoardDef.hpp>

#include <array>

namespace AIO::Game
{
using BoardPlane = std::array<StoneColor, BOARD_SIZE>;

class Board final
{
public:
    Board();
};
}  // namespace AIO::Game

#endif  // AIO_BOARD_HPP
