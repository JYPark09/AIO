#ifndef AIO_MATCH_GAME_HPP
#define AIO_MATCH_GAME_HPP

#include "MatchOptions.hpp"

#include <AIO/Game/BoardDef.hpp>

#include <vector>

struct GameData final
{
    std::vector<AIO::Game::Point> history;
    AIO::Game::StoneColor winColor;
    int winner;
};

GameData RunGame(const MatchOptions& opt, bool p1IsBlack);

#endif  // AIO_MATCH_GAME_HPP
