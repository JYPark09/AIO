#include <AIO/Game/BoardDef.hpp>

#include <algorithm>
#include <cctype>
#include <sstream>

namespace AIO::Game
{
namespace ColorUtil
{
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

StoneColor Str2Color(std::string str)
{
    using namespace std::string_literals;

    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::toupper(c); });

    if (str == "B"s || str == "BLACK"s)
        return P_BLACK;

    if (str == "W"s || str == "WHITE"s)
        return P_WHITE;

    if (str == "NONE"s)
        return P_NONE;

    return P_INVALID;
}
}  // namespace ColorUtil

namespace PointUtil
{
std::string PointStr(Point pt)
{
    if (pt == PASS)
        return "PASS";

    auto [x, y] = Point2XY(pt);

    return PointStr(x, y);
}

std::string PointStr(int x, int y)
{
    static const char* ColStrs = " ABCDEFGHJKLMNOPQRSTUVWXYZ";

    std::stringstream ss;

    ss << ColStrs[x];
    ss << y;

    return ss.str();
}

Point Str2Point(std::string str)
{
    using namespace std::string_literals;

    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::toupper(c); });

    if (str == "PASS")
        return PASS;

    const int x = str[0] - 'A';
    const int y = std::stoi(str.substr((1)));

    return XY2Point(x + 1, y);
}
}  // namespace PointUtil
}  // namespace AIO::Game
