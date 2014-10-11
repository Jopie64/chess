#include "chessboard.h"

using namespace std;

ChessBoard::ChessBoard()
{
}


namespace Chess
{

class Field{};

class BoardImpl : public ChessBoard
{
public:
    BoardImpl(){}

    virtual void print(ostream& os) override
    {
        os << "..." << endl;
    }
};

}//namespace Chess

PChessBoard makeChessBoard()
{
    return make_shared<Chess::BoardImpl>();
}
