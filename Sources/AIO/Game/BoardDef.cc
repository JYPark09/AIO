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
constexpr std::size_t Pt2Idx(std::size_t x, std::size_t y)
{
    return x + y * BOARD_WIDTH;
}

constexpr std::tuple<std::size_t, std::size_t> Idx2Pt(std::size_t idx)
{
    return { idx % BOARD_WIDTH, idx / BOARD_WIDTH };
}

std::string PointStr(std::size_t idx)
{
    auto [x, y] = Idx2Pt(idx);

    return PointStr(x, y);
}

std::string PointStr(std::size_t x, std::size_t y)
{
    static const char* ColStrs = "ABCDEFGHJKLMNOPQRSTUVWXYZ";

    std::stringstream ss;

    ss << ColStrs[x];
    ss << y;

    return ss.str();
}
}  // namespace PointUtil
}  // namespace AIO::Game
