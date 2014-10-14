#include "chessboard.h"
#include <string.h>
#include <sstream>

using namespace std;



namespace Chess
{

ChessBoard::ChessBoard()
{
}

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


ostream& operator <<(ostream& os, const Pos& p)
{
    return os << (char)('A' + p.x) << 1 + (int)p.y;
}

ostream& operator <<(ostream& os, const Move& m)
{
    return os << m.from << (m.capturing ? "x" : "-") << m.to;
}

istream& operator >>(istream& is, Pos& p)
{
    char x;
    is >> x;
    char y;
    is >> y;

    p.x = toupper(x) - 'A';
    p.y = y - '1';

    return is;
}

istream& operator >>(istream& is, Move& m)
{
    Pos from;
    is >> from;
    char dummy;
    is >> dummy;

    Pos to;
    is >> to;

    m.from = from;
    m.to = to;

    return is;
}

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
    inline bool isEmpty() const { return piece() == nothing; }
    inline bool isOfColor(bool white) const { if(isEmpty()) return false; return !white == !color(); }

    unsigned char m_piece;
};

struct Field
{
    Field():turn(true) //White first
    { memset(pieces,0,sizeof(pieces)); }

    static inline int toIx(Pos p) { return p.x + p.y * WIDTH; }
    static inline Pos toPos(int ix) { return Pos(ix%WIDTH, ix/WIDTH); }
    Piece get(int i) const { return pieces[i]; }
    Piece get(Pos pos) const { return pieces[toIx(pos)]; }
    void  set(Pos pos, Piece p) { pieces[toIx(pos)] = p; }

    bool isInside(Pos pos) const { return pos.x >= 0 && pos.x < WIDTH && pos.y >= 0 && pos.y < HEIGHT; }

    void getMoves(T_moves& moves)
    {
        for(int i=0; i < POSITIONS; ++i)
            if(get(i).isOfColor(turn))
                getMoves(moves, i);
    }

    void getMoves(T_moves& moves, Pos pos)
    {
        getMoves(moves, toIx(pos));
    }

    inline int isOkMove(Piece p, Pos to)
    {
        if(!isInside(to))
            return 0;
        Piece onNewPos = get(to);
        if(onNewPos.isEmpty())
            return 1;
        if(!onNewPos.color() == !p.color())
            return 0;
        return 2;
    }

    inline bool addMove(T_moves& moves, Piece p, Pos from, Pos to)
    {
        int ok = isOkMove(p, to);
        if(ok == 0)
            return false;
        moves.emplace_back(Move(from,to,ok != 1));
        return ok == 1;
    }

    inline void addRookMoves(T_moves& moves, Piece p, Pos from)
    {
        for(int i = 0; i < 4; ++i)
        {
            Pos newPos = from;
            while(true)
            {
                switch(i)
                {
                case 0: ++newPos.x; break;
                case 1: --newPos.x; break;
                case 2: ++newPos.y; break;
                case 3: --newPos.y; break;
                }
                if(!addMove(moves,p,from,newPos))
                    break;
            }
        }
    }

    inline void addBishopMoves(T_moves& moves, Piece p, Pos from)
    {
        for(int i = 0; i < 4; ++i)
        {
            Pos newPos = from;
            while(true)
            {
                switch(i)
                {
                case 0: ++newPos.x; ++newPos.y; break;
                case 1: --newPos.x; ++newPos.y; break;
                case 2: ++newPos.x; --newPos.y; break;
                case 3: --newPos.x; --newPos.y; break;
                }
                if(!addMove(moves,p,from,newPos))
                    break;
            }
        }
    }

    void getMoves(T_moves& moves, int i)
    {
        Piece p = get(i);
        Pos pos = toPos(i);
        switch(p.piece())
        {
        default:
        case Piece::nothing: throw runtime_error("Unable to move this piece");
        case Piece::pawn:
        {
            Pos newPos = pos;
            newPos.y += p.color() ? 1 : -1;
            newPos.x -= 1;
            if(isOkMove(p,newPos) == 2)
                moves.emplace_back(Move(pos,newPos,true));
            newPos.x += 2;
            if(isOkMove(p,newPos) == 2)
                moves.emplace_back(Move(pos,newPos,true));
            newPos.x -= 1; //Orig pos
            if(isOkMove(p,newPos) != 1)
                break;
            moves.emplace_back(Move(pos,newPos,false));
            if ((p.color() && pos.y != 1) || (!p.color() && pos.y != 6))
                break; //Not able to do 2 moves forward
            newPos.y += p.color() ? 1 : -1;
            if(isOkMove(p,newPos) != 1)
                break;
            moves.emplace_back(Move(pos,newPos,false));
        }
        break;
        case Piece::rook: addRookMoves(moves,p,pos); break;
        case Piece::knight:
        {
            Pos newPos = pos;
            //2 up
            newPos.y += 2;
            newPos.x += 1;
            addMove(moves,p,pos,newPos);
            newPos.x -= 2;
            addMove(moves,p,pos,newPos);
            //2 down
            newPos.y -= 4;
            addMove(moves,p,pos,newPos);
            newPos.x += 2;
            addMove(moves,p,pos,newPos);
            //2 right
            newPos.y += 1;
            newPos.x += 1;
            addMove(moves,p,pos,newPos);
            newPos.y += 2;
            addMove(moves,p,pos,newPos);
            //2 left
            newPos.x -= 4;
            addMove(moves,p,pos,newPos);
            newPos.y -= 2;
            addMove(moves,p,pos,newPos);
        }
        break;
        case Piece::bishop: addBishopMoves(moves,p,pos); break;
        case Piece::queen:  addBishopMoves(moves,p,pos);
                            addRookMoves  (moves,p,pos); break;
        case Piece::king:
        {
            Pos newPos = pos;
            ++newPos.y;
            addMove(moves,p,pos,newPos);
            ++newPos.x;
            addMove(moves,p,pos,newPos);
            --newPos.y;
            addMove(moves,p,pos,newPos);
            --newPos.y;
            addMove(moves,p,pos,newPos);
            --newPos.x;
            addMove(moves,p,pos,newPos);
            --newPos.x;
            addMove(moves,p,pos,newPos);
            ++newPos.y;
            addMove(moves,p,pos,newPos);
            ++newPos.y;
            addMove(moves,p,pos,newPos);
        }
        break;
        }
    }

    void move(const Move& move)
    {
        set(move.to, get(move.from));
        set(move.from, Piece());
        turn = !turn;
    }

    Piece pieces[POSITIONS];
    bool  turn;
};

#define PW(p) {Piece(true, Piece::p)},
#define PB(p) {Piece(false, Piece::p)},
const Piece INITIAL_FIELD[]=
{
    PW(rook)    PW(knight)  PW(bishop)  PW(queen)   PW(king)    PW(bishop)  PW(knight)  PW(rook)
    PW(pawn)    PW(pawn)    PW(pawn)    PW(pawn)    PW(pawn)    PW(pawn)    PW(pawn)    PW(pawn)
    PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing)
    PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing)
    PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing)
    PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing) PB(nothing)
    PB(pawn)    PB(pawn)    PB(pawn)    PB(pawn)    PB(pawn)    PB(pawn)    PB(pawn)    PB(pawn)
    PB(rook)    PB(knight)  PB(bishop)  PB(queen)   PB(king)    PB(bishop)  PB(knight)  PB(rook)
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
        for(pos.y = 7; pos.y >= 0; --pos.y)
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
        field.turn = true; //White first
    }

    virtual T_moves getMoves(Pos p) override
    {
        if(!field.isInside(p))
            throw runtime_error("Not a valid position");
        Piece piece = field.get(p);
        if(piece.piece() == Piece::nothing)
            throw runtime_error("No piece on this position");
        if(!piece.isOfColor(field.turn))
            throw runtime_error("Not this player's turn");
        T_moves moves;
        field.getMoves(moves, p);
        return moves;
    }

    virtual T_moves getMoves() override
    {
        T_moves moves;
        field.getMoves(moves);
        return moves;
    }

    virtual void move(const Move& move) override
    {
        bool valid = false;
        for(auto &i:getMoves(move.from))
            if(i.to == move.to)
            {
                valid = true;
                break;
            }
        if(!valid)
            throw runtime_error("Not a valid move");
        field.move(move);
    }

    virtual void move(const char* moveStr) override
    {
        Move m;
        istringstream is(moveStr);
        is >> m;
        move(m);
    }

    Field field;
};

PChessBoard makeChessBoard()
{
    return make_shared<BoardImpl>();
}

}//namespace Chess
