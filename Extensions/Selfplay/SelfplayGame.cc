#include "SelfplayGame.hpp"

#include <AIO/Game/Board.hpp>
#include <AIO/Search/SearchEngine.hpp>

using namespace AIO;

std::vector<TrainingData> RunGame(const SelfplayOptions& opt)
{
    std::vector<TrainingData> gameData;

    Search::SearchEngine blackEngine(opt.SearchOpt);
    Search::SearchEngine whiteEngine(opt.SearchOpt);

    Game::Board board;

    int turn = 0;
    while (!board.IsEnd())
    {
        TrainingData data;

        Search::SearchEngine& curPlayer =
            ((turn % 2) ? whiteEngine : blackEngine);

        data.state = Network::StateToTensor(board);
        curPlayer.Search();

        data.pi.resize(Game::BOARD_SIZE);
        std::fill(data.pi.begin(), data.pi.end(), 0.f);

        const Search::TreeNode* root = curPlayer.GetRoot();

        for (Search::TreeNode* child = root->mostLeftChildNode;
             child != nullptr; child = child->rightSiblingNode)
        {
            if (child->action == Game::PASS)
                break;

            data.pi[Game::PointUtil::UnextendedPt(child->action)] =
                child->visits;
        }

        const Game::Point bestMove = curPlayer.GetBestMove();
        blackEngine.Play(bestMove);
        whiteEngine.Play(bestMove);
        board.Play(bestMove);
        data.move = bestMove;

        gameData.emplace_back(data);

        ++turn;
    }

    const Game::StoneColor winner = board.GetWinner();

    const std::size_t gameLength = gameData.size();
    for (std::size_t i = 0; i < gameLength; ++i)
    {
        if (winner == Game::P_NONE)
            gameData[i].z = 0;
        else if (i % 2 == 0 && winner == Game::P_BLACK)
            gameData[i].z = 1;
        else if (i % 2 == 1 && winner == Game::P_WHITE)
            gameData[i].z = 1;
        else
            gameData[i].z = -1;
    }

    return gameData;
}
