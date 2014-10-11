//#include <QCoreApplication>
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
    function<void()> exec;
};

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);

//    return a.exec();

    bool quit = false;


    PChessBoard board = makeChessBoard();

    function<void()> printHelp;
    Command cmd[] = {
        {
            "quit", "q",
            "Exit the application",
            [&]{ quit = true; }
        },
        {
            "print", "p",
            "Print the chess board.",
            [&]{ board->print(cout); }
        },
        {
            "reset", "r",
            "Reset the board to its initial status",
            [&]{ board->reset(); }
        },
        {
            "help", "h",
            "Print this help.",
            [&]{ printHelp(); }
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
        istringstream tok(commandLine);

        string command;
        tok >> command;
        if(command.empty())
            continue;

        bool bExecuted = false;
        for(auto &i:cmd)
        {
            if(command == i.command || command == i.shortcut)
            {
                bExecuted = true;
                i.exec();
            }
        }
        if(!bExecuted)
            cout << "Unknown command: " << command << endl;
    }
    return 0;
}
