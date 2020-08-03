#include <AIO/Network/Preprocess.hpp>

#include <algorithm>

namespace
{
constexpr std::size_t idx(std::size_t c, std::size_t x, std::size_t y)
{
    return (x - 1) + (y - 1) * AIO::Game::BOARD_WIDTH +
           c * AIO::Game::BOARD_SIZE;
}
}  // namespace

namespace AIO::Network
{
Tensor StateToTensor(const Game::Board& state)
{
    Tensor ret(Game::BOARD_SIZE * TENSOR_DIM);
    std::fill(ret.begin(), ret.end(), 0.f);

    const auto& planes = state.GetPlaneHistory();

    const Game::StoneColor cur = state.Current();
    const Game::StoneColor opp = state.Opponent();

    const std::size_t moves = std::min(state.MoveNum(), PAST_MOVES);
    for (std::size_t move = 0; move < moves; ++move)
    {
        const auto& past = *(planes.end() - (moves + 1));

        for (std::size_t y = 1; y <= Game::BOARD_HEIGHT; ++y)
        {
            for (std::size_t x = 1; x <= Game::BOARD_WIDTH; ++x)
            {
                const Game::Point pt = Game::PointUtil::XY2Point(x, y);

                if (past[pt] == cur)
                    ret[idx(2 * move + 0, x, y)] = 1;
                else if (past[pt] == opp)
                    ret[idx(2 * move + 1, x, y)] = 1;
            }
        }
    }

    std::fill(ret.begin() + idx(TENSOR_DIM - 1, 0, 0),
              ret.begin() + idx(TENSOR_DIM, 0, 0),
              static_cast<float>(cur == Game::P_BLACK));

    return ret;
}
}  // namespace AIO::Network
