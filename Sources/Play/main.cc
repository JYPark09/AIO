#include <AIO/Game/Board.hpp>

#include <iostream>

using namespace AIO;

using std::cin;
using std::cout;
using std::endl;

int main()
{
    Game::Board board;

    std::string line;
    while (true)
    {
        cout << "Turn: " << board.MoveNum() << '\n';
        board.ShowBoard(std::cout, true);

        cout << "Play: ";

        std::getline(cin, line);
        const Game::Point pt = Game::PointUtil::Str2Point(line);

        board.Play(pt);

        cout << endl;
    }
}
