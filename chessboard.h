#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <memory>
#include <iostream>
#include <vector>

namespace Chess
{

typedef unsigned short T_hash;

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

struct Piece
{
    enum Enum
    {
        nothing, pawn, rook, knight, bishop, queen, king
    };

    Piece(){}
    Piece(bool):m_piece(0){}
    Piece(bool color, Enum piece):m_piece((color ? 0x80 : 0) | piece) {}


    inline bool color() const { return m_piece & 0x80; }
    inline Enum piece() const { return Enum(m_piece&0xF); }
    inline bool isEmpty() const { return piece() == nothing; }
    inline bool isOfColor(bool white) const { if(isEmpty()) return false; return !white == !color(); }

    unsigned char m_piece;
};

struct Move
{
    Move(){}
    Move(Pos from_, Pos to_):from(from_), to(to_), pfrom(false), pto(false){}

    bool operator<(const Move& that) const
    {
        if(from < that.from) return true;
        if(that.from < from) return false;
        return to < that.to;
    }

    bool capturing() const { return !pto.isEmpty() && !pto.isOfColor(pfrom.color()); }

    Pos from;
    Pos to;
    Piece pfrom;
    Piece pto;
};

struct MoveScore
{
    MoveScore():score(0){}
    MoveScore(Move move_, int score_):move(move_),score(score_){}

    Move move;
    int score;
};

std::ostream& operator <<(std::ostream& os, const Move& m);
std::istream& operator >>(std::istream& is, Move& m);

typedef std::function<bool (Move m)> T_moveCollector;

typedef std::vector<Move> T_moves;

inline T_moveCollector makeMovesInVectorCollector(T_moves& moves, bool turn)
{
    T_moves* pmoves = &moves;
    return [=](Move m)
    {
        if(m.pfrom.isOfColor(turn) && (m.pto.isEmpty() || !m.pto.isOfColor(turn)))
            pmoves->emplace_back(m);
        return true;
    };
}

typedef std::function<void (Move m, int progress, int score)> T_moveProgress;


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
    virtual void    think(const T_moveProgress& moves, int depth) =0;
     //http://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
    virtual std::string
                    fen() const =0;
    virtual void    fen(std::istream& is) =0;
    virtual void    fen(const char* s) =0;
    virtual T_hash  hash() const=0;
};

typedef std::shared_ptr<ChessBoard> PChessBoard;

PChessBoard makeChessBoard();

}

#endif // CHESSBOARD_H
