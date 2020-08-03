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

constexpr std::size_t PASS = BOARD_SIZE;
namespace PointUtil
{
constexpr std::size_t Pt2Idx(std::size_t x, std::size_t y);
constexpr std::tuple<std::size_t, std::size_t> Idx2Pt(std::size_t idx);
std::string PointStr(std::size_t idx);
std::string PointStr(std::size_t x, std::size_t y);
}  // namespace PointUtil
}  // namespace AIO::Game

#endif  // AIO_BOARD_DEF_HPP
