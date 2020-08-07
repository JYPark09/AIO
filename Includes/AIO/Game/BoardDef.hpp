#ifndef AIO_BOARD_DEF_HPP
#define AIO_BOARD_DEF_HPP

#include <array>
#include <cstddef>
#include <string>
#include <tuple>

namespace AIO::Game
{
constexpr std::size_t BOARD_WIDTH = 8;
constexpr std::size_t BOARD_HEIGHT = 8;
constexpr std::size_t BOARD_SIZE = BOARD_HEIGHT * BOARD_WIDTH;

constexpr std::size_t EXTENDED_BOARD_WIDTH = BOARD_WIDTH + 2;
constexpr std::size_t EXTENDED_BOARD_HEIGHT = BOARD_HEIGHT + 2;
constexpr std::size_t EXTENDED_BOARD_SIZE =
    EXTENDED_BOARD_HEIGHT * EXTENDED_BOARD_WIDTH;

using StoneColor = int;
constexpr StoneColor P_BLACK = -1;
constexpr StoneColor P_NONE = 0;
constexpr StoneColor P_WHITE = 1;
constexpr StoneColor P_INVALID = -100;

namespace ColorUtil
{
[[nodiscard]] constexpr StoneColor Opponent(StoneColor color)
{
    if (color == P_INVALID || color == P_NONE)
        return P_INVALID;

    return -1 * color;
}

[[nodiscard]] std::string ColorStr(StoneColor color);
[[nodiscard]] StoneColor Str2Color(std::string str);
}  // namespace ColorUtil

using Point = int;
constexpr Point PASS = EXTENDED_BOARD_SIZE;
constexpr Point INVALID_MOVE = -1;
constexpr std::array<int, 8> Dirs = {
    1,
    -static_cast<int>(EXTENDED_BOARD_WIDTH) + 1,
    -static_cast<int>(EXTENDED_BOARD_WIDTH),
    -static_cast<int>(EXTENDED_BOARD_WIDTH) - 1,
    -1,
    static_cast<int>(EXTENDED_BOARD_WIDTH) - 1,
    static_cast<int>(EXTENDED_BOARD_WIDTH),
    static_cast<int>(EXTENDED_BOARD_WIDTH) + 1
};
namespace PointUtil
{
[[nodiscard]] constexpr Point XY2Point(int x, int y)
{
    return x + y * EXTENDED_BOARD_WIDTH;
}

[[nodiscard]] constexpr std::tuple<int, int> Point2XY(Point pt)
{
    return { pt % static_cast<int>(EXTENDED_BOARD_WIDTH),
             pt / static_cast<int>(EXTENDED_BOARD_WIDTH) };
}

[[nodiscard]] constexpr Point UnextendedPt(Point pt)
{
    auto [x, y] = Point2XY(pt);

    return (x - 1) + (y - 1) * BOARD_WIDTH;
}

[[nodiscard]] std::string PointStr(Point pt);
[[nodiscard]] std::string PointStr(int x, int y);
[[nodiscard]] Point Str2Point(std::string str);
}  // namespace PointUtil
}  // namespace AIO::Game

#endif  // AIO_BOARD_DEF_HPP
