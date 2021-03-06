#include <AIO/Game/Board.hpp>

#include <algorithm>
#include <iomanip>
#include <numeric>

namespace AIO::Game
{
Board::Board()
{
    Clear();
}

void Board::Clear()
{
    std::fill(board_.begin(), board_.end(), P_NONE);

    for (int x = 0; x < EXTENDED_BOARD_WIDTH; ++x)
    {
        At(x, 0) = P_INVALID;
        At(x, EXTENDED_BOARD_HEIGHT - 1) = P_INVALID;
    }

    for (int y = 0; y < EXTENDED_BOARD_HEIGHT; ++y)
    {
        At(0, y) = P_INVALID;
        At(EXTENDED_BOARD_WIDTH - 1, y) = P_INVALID;
    }

    At(BOARD_WIDTH / 2, BOARD_HEIGHT / 2) = P_WHITE;
    At(BOARD_WIDTH / 2 + 1, BOARD_HEIGHT / 2) = P_BLACK;
    At(BOARD_WIDTH / 2, BOARD_HEIGHT / 2 + 1) = P_BLACK;
    At(BOARD_WIDTH / 2 + 1, BOARD_HEIGHT / 2 + 1) = P_WHITE;

    current_ = P_BLACK;

    history_.clear();
    planeHistory_.clear();
    planeHistory_.push_back(board_);
}

bool Board::IsOnBoard(Point pt) const
{
    auto [x, y] = PointUtil::Point2XY(pt);

    return (x >= 0 && y >= 0) && (x < BOARD_WIDTH && y < BOARD_HEIGHT);
}

bool Board::IsValid(Point pt) const
{
    if (pt == PASS)
        return ValidMoves().empty();

    if (pt == INVALID_MOVE || board_[pt] != P_NONE)
        return false;

    const StoneColor opp = Opponent();

    for (const auto dir : Dirs)
    {
        int d;
        for (d = 1;; ++d)
        {
            if (board_[pt + dir * d] != opp)
                break;
        }

        if (d > 1 && board_[pt + dir * d] == current_)
        {
            return true;
        }
    }

    return false;
}

std::size_t Board::MoveNum() const noexcept
{
    return history_.size() + 1;
}

StoneColor Board::At(Point pt) const
{
    return board_[pt];
}

StoneColor Board::At(int x, int y) const
{
    return board_[PointUtil::XY2Point(x, y)];
}

StoneColor& Board::At(Point pt)
{
    return board_[pt];
}

StoneColor& Board::At(int x, int y)
{
    return board_[PointUtil::XY2Point(x, y)];
}

StoneColor Board::Current() const noexcept
{
    return current_;
}

StoneColor Board::Opponent() const noexcept
{
    return ColorUtil::Opponent(current_);
}

int Board::Score() const noexcept
{
    auto [black, white, empty] = calcTerritory();

    return white - black;
}

std::vector<Point> Board::ValidMoves() const
{
    std::vector<Point> ret;

    for (int x = 1; x <= BOARD_WIDTH; ++x)
    {
        for (int y = 1; y <= BOARD_HEIGHT; ++y)
        {
            const auto pt = PointUtil::XY2Point(x, y);

            if (IsValid(pt))
                ret.emplace_back(pt);
        }
    }

    return ret;
}

bool Board::IsEnd() const
{
    if (isEnd_)
        return true;

    auto [black, white, empty] = calcTerritory();

    return (black == 0) || (white == 0) || (empty == 0);
}

StoneColor Board::GetWinner() const
{
    auto [black, white, empty] = calcTerritory();
	if (white == 0)
		return P_BLACK;

	if (black == 0)
		return P_WHITE;

    if (empty > 0)
        return P_INVALID;

    if (black > white)
        return P_BLACK;

    if (black < white)
        return P_WHITE;

    return P_NONE;
}

const std::vector<Point>& Board::GetHistory() const noexcept
{
    return history_;
}

const std::vector<BoardPlane>& Board::GetPlaneHistory() const noexcept
{
    return planeHistory_;
}

void Board::Play(Point pt)
{
    if (pt != PASS)
    {
        board_[pt] = current_;

        flipStones(pt, current_);
    }
    else
    {
        if (!history_.empty() && history_.back() == PASS)
        {
            isEnd_ = true;
        }
    }

    history_.emplace_back(pt);
    planeHistory_.push_back(board_);
    current_ = ColorUtil::Opponent(current_);
}

void Board::Play(int x, int y)
{
    Play(PointUtil::XY2Point(x, y));
}

void Board::Undo()
{
    if (history_.empty())
        return;

    board_ = std::move(*(planeHistory_.end() - 2));
    planeHistory_.erase(planeHistory_.end() - 1);
    history_.erase(history_.end() - 1);
    isEnd_ = false;

    current_ = ColorUtil::Opponent(current_);
}

void Board::ShowBoard(std::ostream& out, bool showValid) const
{
    out << "   ";
    for (int i = 0; i < BOARD_WIDTH; ++i)
    {
        out << static_cast<char>('A' + i) << ' ';
    }

    for (int y = 1; y <= BOARD_HEIGHT; ++y)
    {
        out << '\n' << std::setw(2) << y << ' ';

        for (int x = 1; x <= BOARD_WIDTH; ++x)
        {
            const auto color = At(x, y);

            if (color == P_BLACK)
                out << "B ";
            else if (color == P_WHITE)
                out << "W ";
            else
            {
                if (showValid && IsValid(PointUtil::XY2Point(x, y)))
                    out << "X ";
                else
                    out << "  ";
            }
        }
    }

    out << "   " << ColorUtil::ColorStr(current_) << " to play\n";

    const auto [black, white, empty] = calcTerritory();
    out << "B: " << black << ", W: " << white << '\n';
}

void Board::flipStones(Point pt, StoneColor color)
{
    const StoneColor opp = ColorUtil::Opponent(color);

    for (const auto dir : Dirs)
    {
        int d;
        for (d = 1;; ++d)
        {
            if (board_[pt + dir * d] != opp)
                break;
        }

        if (d > 1 && board_[pt + dir * d] == color)
        {
            for (; d > 0; --d)
            {
                board_[pt + dir * d] = color;
            }
        }
    }
}

std::tuple<int, int, int> Board::calcTerritory() const noexcept
{
    int scores[3] = { 0 };

    for (int y = 1; y <= BOARD_HEIGHT; ++y)
    {
        for (int x = 1; x <= BOARD_WIDTH; ++x)
        {
            ++scores[At(x, y) + 1];
        }
    }

    return { scores[0], scores[2], scores[1] };
}
}  // namespace AIO::Game
