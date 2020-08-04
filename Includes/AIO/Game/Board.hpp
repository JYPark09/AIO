#ifndef AIO_BOARD_HPP
#define AIO_BOARD_HPP

#include <AIO/Game/BoardDef.hpp>

#include <array>
#include <ostream>
#include <vector>

namespace AIO::Game
{
using BoardPlane = std::array<StoneColor, EXTENDED_BOARD_SIZE>;

class Board final
{
 public:
    Board();

    void Clear();

    [[nodiscard]] bool IsOnBoard(Point pt) const;
    [[nodiscard]] bool IsValid(Point pt, StoneColor color) const;

    [[nodiscard]] std::size_t MoveNum() const noexcept;
    [[nodiscard]] StoneColor At(Point pt) const;
    [[nodiscard]] StoneColor At(int x, int y) const;
    [[nodiscard]] StoneColor Current() const noexcept;
    [[nodiscard]] StoneColor Opponent() const noexcept;
    [[nodiscard]] int Score() const noexcept;
    [[nodiscard]] std::vector<Point> ValidMoves() const;

    [[nodiscard]] bool IsEnd() const;
    [[nodiscard]] StoneColor GetWinner() const;

    [[nodiscard]] const std::vector<Point>& GetHistory() const noexcept;
    [[nodiscard]]const std::vector<BoardPlane>& GetPlaneHistory() const noexcept;

    void Play(Point pt);
    void Play(int x, int y);

    void ShowBoard(std::ostream& out, bool showValid = false) const;

 private:
    [[nodiscard]] StoneColor& At(Point pt);
    [[nodiscard]] StoneColor& At(int x, int y);

    void flipStones(Point pt, StoneColor color);
    std::tuple<int, int, int> calcTerritory() const noexcept;

 private:
    BoardPlane board_;
    StoneColor current_{ P_INVALID };

    std::vector<Point> history_;
    std::vector<BoardPlane> planeHistory_;
};
}  // namespace AIO::Game

#endif  // AIO_BOARD_HPP
