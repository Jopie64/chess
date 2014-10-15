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

    bool operator==(const Pos& that) const
    {
        return x == that.x && y == that.y;
    }

    bool operator<(const Pos& that) const
    {
        if(x < that.x) return true;
        if(x > that.x) return false;
        return y < that.y;
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

    bool operator<(const Move& that) const
    {
        if(from < that.from) return true;
        if(that.from < from) return false;
        return to < that.to;
    }

    Pos from;
    Pos to;
    bool capturing;
};

std::ostream& operator <<(std::ostream& os, const Move& m);
std::istream& operator >>(std::istream& is, Move& m);

typedef std::function<void (Move m)> T_moveCollector;

typedef std::vector<Move> T_moves;

inline T_moveCollector makeMovesInVectorCollector(T_moves& moves)
{
    T_moves* pmoves = &moves;
    return [=](Move m){ pmoves->emplace_back(m); };
}


class ChessBoard
{
public:
    ChessBoard();

    virtual void print(std::ostream& os) =0;
    virtual void reset() =0;

    virtual T_moves getMoves(Pos p) =0;
    virtual T_moves getMoves() =0;
    virtual void    move(const Move& move) =0;
    virtual void    move(const char* move) =0;
    virtual void    undo() =0;
    virtual int     evaluate() const=0;
};

typedef std::shared_ptr<ChessBoard> PChessBoard;

PChessBoard makeChessBoard();

}

#endif // CHESSBOARD_H
