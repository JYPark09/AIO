#include <AIO/Game/BoardDef.hpp>

#include <sstream>

namespace AIO::Game
{
namespace ColorUtil
{
constexpr StoneColor Opponent(StoneColor color)
{
    if (color == P_INVALID || color == P_NONE)
        return P_INVALID;

    return -1 * color;
}

std::string ColorStr(StoneColor color)
{
    using namespace std::string_literals;

    switch (color)
    {
        case P_BLACK:
            return "B"s;
        case P_WHITE:
            return "W"s;
        case P_NONE:
            return "NONE"s;

        default:
            return "INVALID"s;
    }
}
}  // namespace ColorUtil

namespace PointUtil
{
constexpr Point XY2Point(int x, int y)
{
    return x + y * BOARD_WIDTH;
}

constexpr std::tuple<int, int> Point2XY(Point idx)
{
    return { idx % static_cast<int>(BOARD_WIDTH),
             idx / static_cast<int>(BOARD_WIDTH) };
}

std::string PointStr(Point pt)
{
    auto [x, y] = Point2XY(pt);

    return PointStr(x, y);
}

std::string PointStr(int x, int y)
{
    static const char* ColStrs = "ABCDEFGHJKLMNOPQRSTUVWXYZ";

    std::stringstream ss;

    ss << ColStrs[x];
    ss << y;

    return ss.str();
}
}  // namespace PointUtil
}  // namespace AIO::Game
