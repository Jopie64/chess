#include "chessboard.h"
#include <string.h>
#include <sstream>
#include <algorithm>

using namespace std;



namespace Chess
{

ChessBoard::ChessBoard()
{
}

const int WIDTH = 8;
const int HEIGHT = 8;
const int POSITIONS = WIDTH * HEIGHT;

const int WINDOWMAX = 0x7FFFFFFF;

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

    template<class T_moveCollector>
    void getMoves(const T_moveCollector& moves) const
    {
        for(int i=0; i < POSITIONS; ++i)
            if(!get(i).isEmpty())
                if(!getMoves(moves, i))
                    return;
    }

    template<class T_moveCollector>
    bool getMoves(const T_moveCollector& moves, Pos pos) const
    {
        return getMoves(moves, toIx(pos));
    }

    inline int isOkMove(Move& m) const
    {
        if(!isInside(m.to))
            return 0;
        m.pto = get(m.to);
        if(m.pto.isEmpty())
            return 1;
        if(!m.pto.color() == !m.pfrom.color())
            return 3;
        return 2;
    }

    template<class T_moveCollector>
    inline bool addMove(const T_moveCollector& moves, Move& m, bool& stop) const
    {
        int ok = isOkMove(m);
        if(ok == 0)
            return false;
        if(!moves(m))
            stop = true;
        return ok == 1;
    }

    template<class T_moveCollector>
    inline void addRookMoves(const T_moveCollector& moves, Move& m, bool& stop) const
    {
        for(int i = 0; i < 4; ++i)
        {
            m.to = m.from;
            while(true)
            {
                if(stop) return;
                switch(i)
                {
                case 0: ++m.to.x; break;
                case 1: --m.to.x; break;
                case 2: ++m.to.y; break;
                case 3: --m.to.y; break;
                }
                if(!addMove(moves,m,stop))
                    break;
            }
        }
    }

    template<class T_moveCollector>
    inline void addBishopMoves(const T_moveCollector& moves, Move& m, bool& stop) const
    {
        for(int i = 0; i < 4; ++i)
        {
            m.to = m.from;
            while(true)
            {
                if(stop) return;
                switch(i)
                {
                case 0: ++m.to.x; ++m.to.y; break;
                case 1: --m.to.x; ++m.to.y; break;
                case 2: ++m.to.x; --m.to.y; break;
                case 3: --m.to.x; --m.to.y; break;
                }
                if(!addMove(moves,m,stop))
                    break;
            }
        }
    }

    template<class T_moveCollector>
    bool getMoves(const T_moveCollector& moves, int i) const
    {
        bool stop = false;
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
                if(!moves(m)) return false;
            m.to.x += 2;
            if(isOkMove(m) == 2)
                if(!moves(m)) return false;
            m.to.x -= 1; //Orig pos
            if(isOkMove(m) != 1)
                break;
            if(!moves(m)) return false;
            if ((m.pfrom.color() && m.from.y != 1) || (!m.pfrom.color() && m.from.y != 6))
                break; //Not able to do 2 moves forward
            m.to.y += m.pfrom.color() ? 1 : -1;
            if(isOkMove(m) != 1)
                break;
            if(!moves(m)) return false;
        }
        break;
        case Piece::rook: addRookMoves(moves,m,stop); if(stop) return false; break;
        case Piece::knight:
        {
            m.to = m.from;
            //2 up
            m.to.y += 2;
            m.to.x += 1;
            addMove(moves,m,stop);if(stop) return false;
            m.to.x -= 2;
            addMove(moves,m,stop);if(stop) return false;
            //2 down
            m.to.y -= 4;
            addMove(moves,m,stop);if(stop) return false;
            m.to.x += 2;
            addMove(moves,m,stop);if(stop) return false;
            //2 right
            m.to.y += 1;
            m.to.x += 1;
            addMove(moves,m,stop);if(stop) return false;
            m.to.y += 2;
            addMove(moves,m,stop);if(stop) return false;
            //2 left
            m.to.x -= 4;
            addMove(moves,m,stop);if(stop) return false;
            m.to.y -= 2;
            addMove(moves,m,stop);if(stop) return false;
        }
        break;
        case Piece::bishop: addBishopMoves(moves,m,stop); if(stop)return false;break;
        case Piece::queen:  addBishopMoves(moves,m,stop); if(stop)return false;
                            addRookMoves  (moves,m,stop); if(stop)return false; break;
        case Piece::king:
        {
            m.to = m.from;
            ++m.to.y;
            addMove(moves,m,stop);if(stop) return false;
            ++m.to.x;
            addMove(moves,m,stop);if(stop) return false;
            --m.to.y;
            addMove(moves,m,stop);if(stop) return false;
            --m.to.y;
            addMove(moves,m,stop);if(stop) return false;
            --m.to.x;
            addMove(moves,m,stop);if(stop) return false;
            --m.to.x;
            addMove(moves,m,stop);if(stop) return false;
            ++m.to.y;
            addMove(moves,m,stop);if(stop) return false;
            ++m.to.y;
            addMove(moves,m,stop);if(stop) return false;
        }
        break;
        }
        return true;
    }

    void move(const Move& move)
    {
        set(move.to, get(move.from));
        set(move.from, Piece(false));
        turn = !turn;
    }

    static int pieceVal(Piece::Enum e)
    {
        switch(e)
        {
        case Piece::pawn: return 1;
        case Piece::knight: return 3;
        case Piece::bishop: return 3;
        case Piece::rook: return 6;
        case Piece::queen: return 10;
        case Piece::king: return 2000000;
        default: return 0;
        }
    }

    int evaluate() const
    {
        int total = 0;
        for(int ix=0; ix < POSITIONS; ++ix)
        {
            auto i = pieces[ix];
            int val = 0;
            int pval = pieceVal(i.piece());
            if(pval == 0)
                continue; //not a piece
            val = pval * 3; //Having a piece is 3 times more worth than being able to capture such a piece
            getMoves([&](Move m)
            {
                bool bIsKing = m.pto.piece() == Piece::king;
                bool bIsDefensive = m.pto.isOfColor(m.pfrom.color());
                if(bIsDefensive && bIsKing)
                    return true; //Defending own king like this is not useful
                ++val;
                if(m.pto.isEmpty())
                    return true;
                //Offensive or defensive moves count even more.
                //Check counts for 2000 points
                if(bIsDefensive)
                    //It is good to defend things from around value 4
                    //Above or below that value, is less of an issue...
                    val += max(0, 3 - abs(pieceVal(m.pto.piece())-4));
                else
                    val += max(0, (bIsKing ? 2000 : pieceVal(m.pto.piece())) - pval);
                return true;
            },ix);
            if(!turn != !i.color())
                val = -val;
            total += val;
        }
        return total;
    }

    void think(const T_moveScore& moves, int maxDepth)
    {
        vector<MoveScore> moveScores;
        for(int i=0; i < POSITIONS; ++i)
            if(get(i).isOfColor(turn))
                getMoves([&](Move m) {
                    if(m.pto.isOfColor(turn))
                        return true;
                    moveScores.emplace_back(m,0);
                    return true;
                }, i);
        for(int depth = 0; depth <= maxDepth; ++depth)
        {
            int a = -WINDOWMAX;
            int b = WINDOWMAX;
            for(auto &mvs : moveScores)
            {
                Move &m = mvs.move;
                Field workField = *this;
                workField.move(m);
                int score = -workField.score(depth, -b, -a);
                bool isSameScore = score == a;
                if(score > a)
                    a = score;
                //alpha/beta pruning causes even or worse scores to be pruned
                //in which case the current highest score is returned. But it is actually
                //probably a worse score, so it should not be the first choice.
                //Thats why 1 is subtracted for follow up 'best' scores, which makes sure
                //it is not the first choice.
                mvs.score = score * 2 - (isSameScore ? 1 : 0);
            }
            sort(moveScores.begin(), moveScores.end(),
                [](const MoveScore& l, const MoveScore& r) {return l.score > r.score;});
        }
        for(auto &mvs : moveScores)
        {
            moves(mvs.move,mvs.score);
        }
    }

    int score(int depth, int a, int b)
    {
        if(depth <= 0)
            return evaluate();
        auto onMove = [&](Move m)
        {
            Field workField = *this;
            workField.move(m);
            int newScore = -workField.score(depth - 1, -b, -a);
            if(newScore > a)
                a = newScore;
            if(a >= b)
                return false; //beta cutoff
            return true;
        };

        for(int i=0; i < POSITIONS; ++i)
            if(get(i).isOfColor(turn))
                if(!getMoves(onMove, i))
                    return a;
        return a;
    }

    static char fenChar(Piece p)
    {
        char c = 0;
        switch(p.piece())
        {
        case Piece::pawn:   c = 'p'; break;
        case Piece::knight: c = 'n'; break;
        case Piece::bishop: c = 'b'; break;
        case Piece::rook:   c = 'r'; break;
        case Piece::queen:  c = 'q'; break;
        case Piece::king:   c = 'k'; break;
        default:            c = 0;
        }
        if(p.color())
            c = toupper(c);
        return c;
    }

    string fen() const
    {
        ostringstream os;
        fen(os);
        return os.str();
    }

    void fen(istream& is)
    {
        memset(&pieces,0,sizeof(pieces));
        turn = true;
        int pos = 0;
        is >> noskipws;
        //skip initial ws
        char c = ' ';
        while(isspace(is.peek()))
            is >> c;
        while(is)
        {
            char c;
            is >> c;
            Piece p(false);
            if(c == ' ')
                break;
            switch(c)
            {
            case '/':
                continue;
            case 'r': p = Piece(false, Piece::rook); break;
            case 'R': p = Piece(true,  Piece::rook); break;
            case 'n': p = Piece(false, Piece::knight); break;
            case 'N': p = Piece(true,  Piece::knight); break;
            case 'b': p = Piece(false, Piece::bishop); break;
            case 'B': p = Piece(true,  Piece::bishop); break;
            case 'q': p = Piece(false, Piece::queen); break;
            case 'Q': p = Piece(true,  Piece::queen); break;
            case 'k': p = Piece(false, Piece::king); break;
            case 'K': p = Piece(true,  Piece::king); break;
            case 'p': p = Piece(false, Piece::pawn); break;
            case 'P': p = Piece(true,  Piece::pawn); break;
            default:
                pos += c - '0';
                continue;
            }
            if(pos >= POSITIONS)
                throw runtime_error("Too many pieces");
            pieces[pos++] = p;
        }
        if(pos != POSITIONS)
            throw runtime_error("Too few pieces");
        string t;
        is >> t;
        turn = t == "w";
    }

    void fen(ostream& os) const
    {
        int count = 0;
        int emptyCount = 0;
        for(auto i:pieces)
        {
            char f = fenChar(i);
            bool nextLine = count != 0 && count % 8 == 0;
            if(emptyCount > 0 && (nextLine || f != 0))
            {
                os << char('0' + emptyCount);
                emptyCount = 0;
            }
            if(nextLine)
                os << '/';
            if(f == 0)
                emptyCount++;
            else
                os << f;
            ++count;
        }
        if(emptyCount > 0)
            os << char('0' + emptyCount);
        os << ' ';
        if(turn)
            os << 'w';
        else
            os << 'b';
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

    virtual void undo() override
    {
        if(fields.size() <= 1)
            throw runtime_error("There is no undo buffer left");
        fields.resize(fields.size()-1);
    }

    virtual int evaluate() const override
    {
        return field().evaluate();
    }

    virtual void think(const T_moveScore& moves, int depth) override
    {
        field().think(moves, depth);
    }

    virtual string fen() const override
    {
        return field().fen();
    }

    virtual void fen(istream& is) override
    {
        return field().fen(is);
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
