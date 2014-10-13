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

void testException(const char* file, int line, const char* info, function<void()> f)
{
    bool exceptionThrown = false;
    try
    {
        f();
    }
    catch(exception&)
    {
        exceptionThrown = true;
    }
    if(!exceptionThrown)
        testerror(file,line,"Exception expected", info);
}

#define TEST_ASSERT(p) if(!(p)) testerror(__FILE__,__LINE__,"Assertion failed", #p)
#define TEST_EQUAL(act,exp) if((exp) != (act)) testErrorEqual(__FILE__,__LINE__,#act,exp,act)
#define TEST_EXCEPTION(f) testException(__FILE__,__LINE__,#f,f)

string isSameMoves(T_moves m1, T_moves m2)
{
    sort(m1.begin(),m1.end());
    sort(m2.begin(),m2.end());
    T_moves diff;
    set_difference(m1.begin(), m1.end(), m2.begin(), m2.end(), inserter(diff, diff.end()));
    set_difference(m2.begin(), m2.end(), m1.begin(), m1.end(), inserter(diff, diff.end()));
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
                               "B1-C3,B1-A3,G1-H3,G1-F3," // <-- Knights
                               "A2-A3,A2-A4,B2-B3,B2-B4,C2-C3,C2-C4,D2-D3,D2-D4,E2-E3,E2-E4,F2-F3,F2-F4,G2-G3,"
                               "G2-G4,H2-H3,H2-H4")), "");

    TEST_EXCEPTION([&]{ board->move("A2-A5"); });
    TEST_EXCEPTION([&]{ board->move("A2-A1"); });
    TEST_EXCEPTION([&]{ board->move("A2-A2"); });
    TEST_EXCEPTION([&]{ board->move("A2-B2"); });
    TEST_EXCEPTION([&]{ board->move("A2-B3"); });

    board->move("A2-A4");

    TEST_EXCEPTION([&]{ board->move("A7-A4"); });
    TEST_EXCEPTION([&]{ board->move("A7-A7"); });
    TEST_EXCEPTION([&]{ board->move("A7-A8"); });
    TEST_EXCEPTION([&]{ board->move("A7-B7"); });
    TEST_EXCEPTION([&]{ board->move("A7-B6"); });

    board->move("A7-A5");

    TEST_EQUAL(isSameMoves(board->getMoves(), parseMoves(
                               "B1-C3,B1-A3,G1-H3,G1-F3," // <-- Knights
                               "A1-A2,A1-A3,B2-B3,B2-B4,C2-C3,C2-C4,D2-D3,D2-D4,E2-E3,E2-E4,F2-F3,F2-F4,G2-G3,"
                               "G2-G4,H2-H3,H2-H4")), "");

    board->move("A1-A3"); //white rook up

    TEST_EQUAL(isSameMoves(board->getMoves(), parseMoves(
                               "B8-A6,B8-C6,G8-F6,G8-H6," // <-- Knights
                               "A8-A6,A8-A7,B7-B5,B7-B6,C7-C5,C7-C6,D7-D5,D7-D6,"
                               "E7-E5,E7-E6,F7-F5,F7-F6,G7-G5,G7-G6,H7-H5,H7-H6")), "");

    board->move("A8-A6"); //black rook down
    board->move("A3-E3"); //white rook right
    board->move("A6-C6"); //black rook right

    TEST_EQUAL(isSameMoves(board->getMoves(), parseMoves(
                               "B1-C3,B1-A3,G1-H3,G1-F3," // <-- Knights
                               "B2-B3,B2-B4,C2-C3,C2-C4,D2-D3,D2-D4,E3-A3,E3-B3,E3-C3,E3-D3,"
                               "E3-E4,E3-E5,E3-E6,E3xE7,E3-F3,E3-G3,E3-H3,F2-F3,F2-F4,G2-G3,"
                               "G2-G4,H2-H3,H2-H4")), "");
    TEST_EXCEPTION([&]{ board->move("E3-E3"); }); //white rook should not be able to move to own position
    TEST_EXCEPTION([&]{ board->move("E3-E8"); }); //white rook should not be able to capture black king
    TEST_EXCEPTION([&]{ board->move("E3-E2"); }); //white rook should not be able to capture own pawn

    board->move("E3-E6"); //white rook up
    board->move("B7-B5"); //other pawn down
    board->move("A4-B5"); //capture pawn
    board->move("A5-A4"); //pawn down

    TEST_EQUAL(isSameMoves(board->getMoves(), parseMoves(
                               "B1-C3,B1-A3,G1-H3,G1-F3," // <-- Knights
                               "B2-B3,B2-B4,B5-B6,B5xC6,C2-C3,C2-C4,D2-D3,D2-D4,E2-E3,E2-E4,"
                               "E6xC6,E6-D6,E6-E3,E6-E4,E6-E5,E6xE7,E6-F6,E6-G6,E6-H6," //<-- rook moves
                               "F2-F3,F2-F4,G2-G3,G2-G4,H2-H3,H2-H4")),"");
    board->move("B1-C3"); //move white knight
    TEST_EXCEPTION([&]{ board->move("B8-C6"); });//black knight cant capture own pawn
    board->move("G8-F6"); //move black knight
    TEST_EQUAL(isSameMoves(board->getMoves(), parseMoves(
                               "C3-A2,C3xA4,C3-B1,C3-D5,C3-E4,G1-F3,G1-H3," // <-- knights
                               "B2-B3,B2-B4,B5-B6,B5xC6,D2-D3,D2-D4,E2-E3,E2-E4,"
                               "E6xC6,E6-D6,E6-E3,E6-E4,E6-E5,E6xE7,E6xF6," //<-- rook moves
                               "F2-F3,F2-F4,G2-G3,G2-G4,H2-H3,H2-H4")),"");

    //board->print(cout);
}

}//namespace ChessTest
