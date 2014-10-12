#include <iostream>
#include <functional>
#include <sstream>
#include <iomanip>
#include "chessboard.h"

using namespace std;

struct Command
{
    string command;
    string shortcut;
    string description;
    function<void(istream& params)> exec;
};

int main(int argc, char *argv[])
{
    using namespace Chess;

    bool quit = false;
    PChessBoard board = makeChessBoard();
    function<void()> printHelp;
    T_moves moves;

    Command cmd[] = {
        {
            "help", "h",
            "Print this help.",
            [&](istream&){ printHelp(); }
        },
        {
            "quit", "q",
            "Exit the application",
            [&](istream&){ quit = true; }
        },
        {
            "print", "p",
            "Print the chess board.",
            [&](istream&){ board->print(cout); }
        },
        {
            "reset", "r",
            "Reset the board to its initial status",
            [&](istream&){ board->reset(); }
        },
        {
            "moves", "",
            "Show moves of given piece",
            [&](istream& params)
            {
                if(params.peek() != char_traits<char>::eof())
                {
                    Pos pos;
                    params >> pos;
                    moves = board->getMoves(pos);
                }
                else
                    moves = board->getMoves();
                int count = 0;
                for(auto &i:moves)
                {
                    if(count != 0)
                        cout << ", ";
                    cout << ++count << ". " << i;
                }
                if(count == 0)
                    cout << "No possible moves for piece on this position.";
                cout << endl;
            }
        }
    };

    printHelp = [&]
    {
        for(auto &i:cmd)
            cout << setw(10) << left << i.command << setw(0) << i.description << endl;
    };

    printHelp();

    string commandLine;
    while(!quit)
    {
        cout << "> " << flush;
        getline(cin, commandLine);
        istringstream params(commandLine);

        string command;
        params >> command;
        if(command.empty())
            continue;

        bool bExecuted = false;
        try
        {
            for(auto &i:cmd)
            {
                if(command == i.command || command == i.shortcut)
                {
                    bExecuted = true;
                    i.exec(params);
                    break;
                }
            }
            if(!bExecuted)
            {
                std::ostringstream msg;
                msg << "Unknown command: " << command;
                throw runtime_error(msg.str());
            }
        }
        catch(runtime_error& e)
        {
            cout << "Error: " << e.what() << endl;
        }
    }
    return 0;
}
