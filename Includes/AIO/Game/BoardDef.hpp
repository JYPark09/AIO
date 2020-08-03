#ifndef AIO_BOARD_DEF_HPP
#define AIO_BOARD_DEF_HPP

#include <cstddef>
#include <string>
#include <tuple>

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

namespace ColorUtil
{
constexpr StoneColor Opponent(StoneColor color);
std::string ColorStr(StoneColor color);
}  // namespace ColorUtil

using Point = int;
constexpr Point PASS = BOARD_SIZE;
namespace PointUtil
{
constexpr Point XY2Point(int x, int y);
constexpr std::tuple<int, int> Point2XY(Point pt);
std::string PointStr(Point pt);
std::string PointStr(int x, int y);
}  // namespace PointUtil
}  // namespace AIO::Game

#endif  // AIO_BOARD_DEF_HPP
