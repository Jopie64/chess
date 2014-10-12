#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <memory>
#include <iostream>
#include <vector>

namespace Chess
{

struct Pos
{
    Pos(char X=-1, char Y=-1):x(X),y(Y){}
    char x;
    char y;
    Pos& operator-=(const Pos& that)
    {
        x -= that.x;
        y -= that.y;
        return *this;
    }
    Pos& operator+=(const Pos& that)
    {
        x += that.x;
        y += that.y;
        return *this;
    }

    Pos operator-(const Pos& that) const
    {
        Pos newPos(*this);
        newPos -= that;
        return newPos;
    }
    Pos operator+(const Pos& that) const
    {
        Pos newPos(*this);
        newPos += that;
        return newPos;
    }
    bool isValid() const { return x >= 0; }
    void invalidate() { x = -1; }

    void swapXY() { std::swap(x,y); }
};

std::ostream& operator <<(std::ostream& os, const Pos& p);
std::istream& operator >>(std::istream& is, Pos& m);

struct Move
{
    Move():capturing(false){}
    Move(Pos from_, Pos to_, bool capturing_ = false):from(from_), to(to_), capturing(capturing_){}
    Pos from;
    Pos to;
    bool capturing;
};

std::ostream& operator <<(std::ostream& os, const Move& m);
std::istream& operator >>(std::ostream& is, Move& m);

typedef std::vector<Move> T_moves;

class ChessBoard
{
public:
    ChessBoard();

    virtual void print(std::ostream& os) =0;
    virtual void reset() =0;

    virtual T_moves getMoves(Pos p) =0;
    virtual T_moves getMoves() =0;
};

typedef std::shared_ptr<ChessBoard> PChessBoard;

PChessBoard makeChessBoard();

}

#endif // CHESSBOARD_H
