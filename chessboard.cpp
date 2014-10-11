#include "chessboard.h"
#include <string.h>

using namespace std;

ChessBoard::ChessBoard()
{
}


namespace Chess
{

const int WIDTH = 8;
const int HEIGHT = 8;
const int POSITIONS = WIDTH * HEIGHT;

const int AsciiPieceWidth = 5;
const int AsciiPieceHeight = 3;
const int AsciiPieceCount = 8;

const char* AsciiPieces =
        "     :::::                      +  \\\\^//"
        "     :::::  _   III  ,o/  [ ]  ( )  ,o/ "
        "     :::::_(_)__[_]__[_]__(_)_[___](___)"
        "     :::::                      +  \\\\^//"
        "     :::::  _   III  ,o/  [ ]  ( )  ,o/ "
        "     :::::_(#)__[#]__[#]__(#)_[_#_](_#_)";


struct Pos
{
    Pos(int X=-1, int Y=-1):x(X),y(Y){}
    int x;
    int y;
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

    void swapXY() { swap(x,y); }
};

struct Piece
{
    enum Enum
    {
        nothing, pawn, rook, knight, bishop, queen, king
    };

    Piece(){}
    Piece(bool color, Enum piece):m_piece((color ? 0x80 : 0) | piece) {}


    inline bool color() const { return m_piece & 0x80; }
    inline Enum piece() const { return Enum(m_piece&0xF); }

    unsigned char m_piece;
};

struct Field
{
    Field(){ memset(pieces,0,sizeof(pieces)); }

    static inline int ix(Pos p) { return p.x + p.y * WIDTH; }
    Piece get(Pos p) const { return pieces[ix(p)]; }

    Piece pieces[POSITIONS];
};

#define PW(p) {Piece(true, Piece::p)},
#define PB(p) {Piece(false, Piece::p)},
const Piece INITIAL_FIELD[]=
{
    PW(rook)    PW(bishop)  PW(knight)  PW(king)    PW(queen)   PW(knight)  PW(bishop)  PW(rook)
    PW(pawn)    PW(pawn)    PW(pawn)    PW(pawn)    PW(pawn)    PW(pawn)    PW(pawn)    PW(pawn)
    PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing)
    PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing)
    PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing)
    PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing)
    PB(pawn)    PB(pawn)    PB(pawn)    PB(pawn)    PB(pawn)    PB(pawn)    PB(pawn)    PB(pawn)
    PB(rook)    PB(bishop)  PB(knight)  PB(king)    PB(queen)   PB(knight)  PB(bishop)  PB(rook)
};
#undef PW
#undef PB

class BoardImpl : public ChessBoard
{
public:
    BoardImpl(){}

    virtual void print(ostream& os) override
    {
        Pos pos;
        for(pos.y = 0; pos.y < 8; ++pos.y)
            for(int line = 0; line < AsciiPieceHeight; ++line)
            {
                for(pos.x = 0; pos.x < 8; ++pos.x)
                {
                    Piece p = field.get(pos);
                    int colorOffset = p.color() ? AsciiPieceCount * AsciiPieceWidth * AsciiPieceHeight : 0;
                    int asciiPieceNr = (pos.x + pos.y) % 2;
                    if(p.piece() != Piece::nothing)
                        asciiPieceNr = p.piece() + 1;
                    size_t asciiStrPos =
                            AsciiPieceCount * AsciiPieceWidth * line +
                            AsciiPieceWidth * asciiPieceNr + colorOffset;
                    os.write(AsciiPieces + asciiStrPos, AsciiPieceWidth);
                }
                os << endl;
            }
    }

    virtual void reset() override
    {
        memcpy(field.pieces, INITIAL_FIELD, sizeof(field.pieces));
    }

    Field field;
};

}//namespace Chess

PChessBoard makeChessBoard()
{
    return make_shared<Chess::BoardImpl>();
}
