#include <AIO/Game/BoardDef.hpp>

namespace AIO::Game
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
}  // namespace AIO::Game
