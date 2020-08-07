#include "MatchGame.hpp"

#include <AIO/Game/Board.hpp>
#include <AIO/Search/SearchEngine.hpp>

using namespace AIO;

GameData RunGame(const MatchOptions& opt, bool p1IsBlack)
{
    GameData gameData;

    Search::SearchEngine engine1(opt.PlayerOpt1);
    Search::SearchEngine engine2(opt.PlayerOpt2);

    Search::SearchEngine& blackEngine = (p1IsBlack ? engine1 : engine2);
    Search::SearchEngine& whiteEngine = (p1IsBlack ? engine2 : engine1);

    Game::Board board;

    int turn = 0;
    while (!board.IsEnd())
    {
        Search::SearchEngine& curPlayer =
            ((turn % 2) ? whiteEngine : blackEngine);

        curPlayer.Search();

        const Game::Point bestMove = curPlayer.GetBestMove();
        blackEngine.Play(bestMove);
        whiteEngine.Play(bestMove);
        board.Play(bestMove);

        gameData.history.emplace_back(bestMove);

        ++turn;
    }

    const Game::StoneColor winner = board.GetWinner();
    gameData.winColor = winner;

    if ((p1IsBlack && winner == Game::P_BLACK) ||
             (!p1IsBlack && winner == Game::P_WHITE))
    {
        gameData.winner = 1;
    }
    else if ((p1IsBlack && winner == Game::P_WHITE) ||
             (!p1IsBlack && winner == Game::P_BLACK))
    {
        gameData.winner = 2;
    }
    else
    {
        gameData.winner = 0;
    }

    return gameData;
}
