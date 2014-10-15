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
    return os << m.from << (m.capturing() ? "x" : "-") << m.to;
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

    void getMoves(const T_moveCollector& moves)
    {
        for(int i=0; i < POSITIONS; ++i)
            if(!get(i).isEmpty())
                getMoves(moves, i);
    }

    void getMoves(const T_moveCollector& moves, Pos pos)
    {
        getMoves(moves, toIx(pos));
    }

    inline int isOkMove(Move& m)
    {
        if(!isInside(m.to))
            return 0;
        m.pto = get(m.to);
        if(m.pto.isEmpty())
            return 1;
        if(!m.pto.color() == !m.pfrom.color())
            return 0;
        return 2;
    }

    inline bool addMove(const T_moveCollector& moves, Move& m)
    {
        int ok = isOkMove(m);
        if(ok == 0)
            return false;
        moves(m);
        return ok == 1;
    }

    inline void addRookMoves(const T_moveCollector& moves, Move& m)
    {
        for(int i = 0; i < 4; ++i)
        {
            m.to = m.from;
            while(true)
            {
                switch(i)
                {
                case 0: ++m.to.x; break;
                case 1: --m.to.x; break;
                case 2: ++m.to.y; break;
                case 3: --m.to.y; break;
                }
                if(!addMove(moves,m))
                    break;
            }
        }
    }

    inline void addBishopMoves(const T_moveCollector& moves, Move& m)
    {
        for(int i = 0; i < 4; ++i)
        {
            m.to = m.from;
            while(true)
            {
                switch(i)
                {
                case 0: ++m.to.x; ++m.to.y; break;
                case 1: --m.to.x; ++m.to.y; break;
                case 2: ++m.to.x; --m.to.y; break;
                case 3: --m.to.x; --m.to.y; break;
                }
                if(!addMove(moves,m))
                    break;
            }
        }
    }

    void getMoves(const T_moveCollector& moves, int i)
    {
        Move m;
        m.from = toPos(i);
        m.pfrom = get(i);
        switch(m.pfrom.piece())
        {
        default:
        case Piece::nothing: throw runtime_error("Unable to move this piece");
        case Piece::pawn:
        {
            m.to = m.from;
            m.to.y += m.pfrom.color() ? 1 : -1;
            m.to.x -= 1;
            if(isOkMove(m) == 2)
                moves(m);
            m.to.x += 2;
            if(isOkMove(m) == 2)
                moves(m);
            m.to.x -= 1; //Orig pos
            if(isOkMove(m) != 1)
                break;
            moves(m);
            if ((m.pfrom.color() && m.from.y != 1) || (!m.pfrom.color() && m.from.y != 6))
                break; //Not able to do 2 moves forward
            m.to.y += m.pfrom.color() ? 1 : -1;
            if(isOkMove(m) != 1)
                break;
            moves(m);
        }
        break;
        case Piece::rook: addRookMoves(moves,m); break;
        case Piece::knight:
        {
            m.to = m.from;
            //2 up
            m.to.y += 2;
            m.to.x += 1;
            addMove(moves,m);
            m.to.x -= 2;
            addMove(moves,m);
            //2 down
            m.to.y -= 4;
            addMove(moves,m);
            m.to.x += 2;
            addMove(moves,m);
            //2 right
            m.to.y += 1;
            m.to.x += 1;
            addMove(moves,m);
            m.to.y += 2;
            addMove(moves,m);
            //2 left
            m.to.x -= 4;
            addMove(moves,m);
            m.to.y -= 2;
            addMove(moves,m);
        }
        break;
        case Piece::bishop: addBishopMoves(moves,m); break;
        case Piece::queen:  addBishopMoves(moves,m);
                            addRookMoves  (moves,m); break;
        case Piece::king:
        {
            m.to = m.from;
            ++m.to.y;
            addMove(moves,m);
            ++m.to.x;
            addMove(moves,m);
            --m.to.y;
            addMove(moves,m);
            --m.to.y;
            addMove(moves,m);
            --m.to.x;
            addMove(moves,m);
            --m.to.x;
            addMove(moves,m);
            ++m.to.y;
            addMove(moves,m);
            ++m.to.y;
            addMove(moves,m);
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

    int evaluate() const
    {
        int total = 0;
        for(auto i:pieces)
        {
            int val = 0;
            switch(i.piece())
            {
            case Piece::pawn: val = 1; break;
            case Piece::knight: val = 3; break;
            case Piece::bishop: val = 3; break;
            case Piece::rook: val = 6; break;
            case Piece::queen: val = 10; break;
            case Piece::king: val = 2000000; break;
            default: continue;
            }
            if(!turn != !i.color())
                val = -val;
            total += val;
        }
        return total;
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
    BoardImpl(){reset();}

    virtual void print(ostream& os) override
    {
        Pos pos;
        for(pos.y = 7; pos.y >= 0; --pos.y)
            for(int line = 0; line < AsciiPieceHeight; ++line)
            {
                for(pos.x = 0; pos.x < 8; ++pos.x)
                {
                    Piece p = field().get(pos);
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
        fields.clear();
        fields.emplace_back();
        memcpy(field().pieces, INITIAL_FIELD, sizeof(field().pieces));
        field().turn = true; //White first
    }

    virtual T_moves getMoves(Pos p) override
    {
        if(!field().isInside(p))
            throw runtime_error("Not a valid position");
        Piece piece = field().get(p);
        if(piece.piece() == Piece::nothing)
            throw runtime_error("No piece on this position");
        if(!piece.isOfColor(field().turn))
            throw runtime_error("Not this player's turn");
        T_moves moves;
        field().getMoves(makeMovesInVectorCollector(moves, field().turn), p);
        return moves;
    }

    virtual T_moves getMoves() override
    {
        T_moves moves;
        field().getMoves(makeMovesInVectorCollector(moves, field().turn));
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
        fields.emplace_back(field());
        field().move(move);
    }

    virtual void move(const char* moveStr) override
    {
        Move m;
        istringstream is(moveStr);
        is >> m;
        move(m);
    }

    virtual void undo()
    {
        if(fields.size() <= 1)
            throw runtime_error("There is no undo buffer left");
        fields.resize(fields.size()-1);
    }

    virtual int evaluate() const
    {
        return field().evaluate();
    }

    Field& field() { return fields.back(); }
    const Field& field() const { return fields.back(); }

    vector<Field> fields;
};

PChessBoard makeChessBoard()
{
    return make_shared<BoardImpl>();
}

}//namespace Chess
