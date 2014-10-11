#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <memory>
#include <iostream>

class ChessBoard
{
public:
    ChessBoard();

    virtual void print(std::ostream& os) =0;
};

typedef std::shared_ptr<ChessBoard> PChessBoard;

PChessBoard makeChessBoard();

#endif // CHESSBOARD_H
