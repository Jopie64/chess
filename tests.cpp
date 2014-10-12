#include <iostream>
#include <functional>
#include <algorithm>
#include <sstream>
#include "chessboard.h"

using namespace std;

namespace ChessTest
{

using namespace Chess;

void testerror(const char* file, int line, const char* what, const char* data)
{
    cout << file << "(" << line << "): " << what << endl << data << endl;
}

template<class T1, class T2>
void testErrorEqual(const char* file, int line, const char* what, const T1& expect, const T2& actual)
{
    ostringstream msg;
    msg << "content of:     " << what << endl
        << "expected to be: " << expect << endl
        << "actual:         " << actual;
    testerror(file,line,"Not equal", msg.str().c_str());
}

#define TEST_ASSERT(p) if(!(p)) testerror(__FILE__,__LINE__,"Assertion failed", #p)
#define TEST_EQUAL(act,exp) if((exp) != (act)) testErrorEqual(__FILE__,__LINE__,#act,exp,act);

string isSameMoves(T_moves m1, T_moves m2)
{
    sort(m1.begin(),m1.end());
    sort(m2.begin(),m2.end());
    T_moves diff;
    set_difference(m1.begin(), m1.end(), m2.begin(), m2.end(), inserter(diff, diff.end()));
    ostringstream ret;
    for(auto &i:diff)
        ret << i << ",";
    return ret.str();
}

T_moves parseMoves(const char* movesStr)
{
    istringstream W_is(movesStr);

    T_moves ret;
    while(W_is)
    {
        Move mov;
        W_is >> mov;
        ret.emplace_back(mov);
        char c;
        W_is >> c;
    }
    return ret;
}

void test()
{
    using namespace Chess;

    PChessBoard board = makeChessBoard();

    T_moves expectedMoves;

    TEST_EQUAL(isSameMoves(board->getMoves(), expectedMoves), ""); // No moves expected at all

    board->reset();

    TEST_EQUAL(isSameMoves(board->getMoves(), parseMoves(
                               "A2-A3,A2-A4,B2-B3,B2-B4,C2-C3,C2-C4,D2-D3,D2-D4,E2-E3,E2-E4,F2-F3,F2-F4,G2-G3,"
                               "G2-G4,H2-H3,H2-H4,")), "");

    board->move("A2-A4");
}

}//namespace ChessTest
