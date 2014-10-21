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

string checkFenIo(string fenIn)
{
    PChessBoard board = makeChessBoard();
    istringstream sFenIn(fenIn);
    board->fen(sFenIn);
    string fenOut = board->fen();
    if(fenOut != fenIn)
        return fenOut;
    return "";
}

void test()
{
    using namespace Chess;

    PChessBoard board = makeChessBoard();

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

    board->move("D2-D3"); //move white pawn, free bishop
    board->move("D7-D6"); //move black pawn, free bishop which was already free
    TEST_EQUAL(isSameMoves(board->getMoves(), parseMoves(
                               "E1-D2," //<-- king
                               "D1-D2," //<-- queen
                               "C1-D2,C1-E3,C1-F4,C1-G5,C1-H6," //<-- bishop
                               "C3-A2,C3xA4,C3-B1,C3-D5,C3-E4,G1-F3,G1-H3," //<-- knights
                               "B2-B3,B2-B4,B5-B6,B5xC6,D3-D4,E2-E3,E2-E4,"
                               "E6xD6,E6-E3,E6-E4,E6-E5,E6xE7,E6xF6," //<-- rook moves
                               "F2-F3,F2-F4,G2-G3,G2-G4,H2-H3,H2-H4")),"");
    board->move("D1-D2"); //move queen up
    board->move("C8-E6"); //black bishop captures rook
    board->move("D2-F4"); //move queen
    board->move("E6-B3"); //move bishop
    TEST_EQUAL(isSameMoves(board->getMoves(), parseMoves(
                               "E1-D1,E1-D2," //<-- king
                               "F4xA4,F4-B4,F4-C4,F4-D2,F4-D4,F4xD6,F4-E3,F4-E4,F4-E5,F4-F3,F4-F5,F4xF6,F4-G3,F4-G4,F4-G5,F4-H4,F4-H6," //<-- queen
                               "C1-D2,C1-E3," //<-- bishop
                               "C3-A2,C3-D1,C3xA4,C3-B1,C3-D5,C3-E4,G1-F3,G1-H3," //<-- knights
                               "C2xB3,B5-B6,B5xC6,D3-D4,E2-E3,E2-E4,"
                               "" //<-- rook moves. Rook is gone...
                               "F2-F3,G2-G3,G2-G4,H2-H3,H2-H4")),"");

    /*
    :::::     :::::  +  \\^//     :::::
    ::::: ,o/ ::::: ( )  ,o/  [ ] ::::: III
    :::::_[_]_:::::[___](___)_(_)_:::::_[_]_
         :::::     :::::
         :::::  _  :::::  _    _    _    _
         :::::_(_)_:::::_(_)__(_)__(_)__(_)_
    :::::               :::::     :::::
    :::::      III   _  ::::: ,o/ :::::
    :::::     _[_]__(_)_:::::_[_]_:::::
                   :::::     :::::     :::::
           _       :::::     :::::     :::::
         _(#)_     :::::     :::::     :::::
              :::::     :::::  +  :::::
      _       :::::     ::::: ( ) :::::
    _(_)_     :::::     :::::[_#_]:::::
                             :::::     :::::
          [ ]  ,o/   _       :::::     :::::
         _(_)__[#]__(#)_     :::::     :::::
    :::::
    :::::  _    _         _    _    _    _
    :::::_(#)__(#)_     _(#)__(#)__(#)__(#)_
         :::::     :::::\\^//
         ::::: [ ] ::::: ,o/  [ ]  ,o/  III
         :::::_(#)_:::::(_#_)_(#)__[#]__[#]_
    */
    const char* endFen = "2B1KBNR/1PP1PPPP/1bNP4/p4Q2/1P6/2rp1n2/2p1pppp/1n1qkb1r w";
    T_hash endHash = board->hash();

    TEST_EQUAL(board->fen(), endFen);

    TEST_EQUAL(checkFenIo("8/8/8/8/8/8/8/8 b"),"");
    TEST_EQUAL(checkFenIo("2B1KBNR/1PP1PPPP/1bNP4/p4Q2/1P6/2rp1n2/2p1pppp/1n1qkb1r w"),"");

    //**** Test board hashes
    board->reset();
    TEST_ASSERT(board->hash() != endHash);

    board->fen(endFen);
    TEST_EQUAL(board->hash(), endHash);

    board->reset();

    // Compare hash of resetted chess board
    PChessBoard board2 = makeChessBoard();
    board2->fen(board->fen().c_str());
    TEST_EQUAL(board2->hash(), board->hash());

    // Compare hash of clear chess board
    board2 = makeChessBoard();
    board->fen(board2->fen().c_str());
    TEST_EQUAL(board2->hash(), board->hash());


    // Compare hash of chess board with single move
    board = makeChessBoard();
    board->fen("K7/8/8/8/8/8/8/k7 w");
    board->move("A1-B1");

    board2 = makeChessBoard();
    board2->fen("1K6/8/8/8/8/8/8/k7 b");
    TEST_EQUAL(board->fen(),board2->fen());
    TEST_EQUAL(board->hash(),board2->hash());

    //**** Test evaluate
    board = makeChessBoard();
    board->fen("K7/8/8/8/8/8/8/k7 w");

    //no win: 0
    TEST_EQUAL(board->evaluate(),0);

    //wins:
    board->fen("K7/8/8/8/8/8/8/q7 w");
    TEST_ASSERT(board->evaluate() > 200000);
    board->fen("K7/8/8/8/8/8/8/q7 b");
    TEST_ASSERT(board->evaluate() < 200000);

    board->fen("k7/8/8/8/8/8/8/Q7 b");
    TEST_ASSERT(board->evaluate() > 200000);
    board->fen("k7/8/8/8/8/8/8/Q7 w");
    TEST_ASSERT(board->evaluate() < 200000);

//  cout << board->fen() << endl;
//  board->print(cout);
}

}//namespace ChessTest
