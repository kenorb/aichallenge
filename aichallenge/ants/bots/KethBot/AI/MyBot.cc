#ifndef __DEBUG
    #ifdef __ASSERT
        #undef __ASSERT
    #endif

    #ifdef __JSON
        #undef __JSON
    #endif
#endif

#include "Bot.h"
#include "globals.h"
#include "Optimizer.h"

#include <iostream>
#include <fstream>
#include <string>

/*
    This program will play a single game of Ants while communicating with
    the engine via standard input and output.

    The function "makeMoves()" in Bot.cc is where it makes the moves
    each turn and is probably the best place to start exploring. You are
    allowed to edit any part of any of the files, remove them, or add your
    own, provided you continue conforming to the input and output format
    outlined on the specifications page at http://www.ai-contest.com .
*/


int main(int argc, char *argv[])
{
    cout.sync_with_stdio(0);

    #ifdef __OPTIMIZER_TEST
        optimizer.testOptimizer();
        cin.ignore(1);
    #else
        if (argc >= 3 && std::string(argv[1]) == "viewlog") {
            string line;
            ifstream f(argv[2]);
            if (f.is_open()) {
                while (f.good())
                {
                    getline(f,line);
                    cout << line << endl;
                }
                f.close();
            }

            // Wait for a key
            cin.ignore(1);
        } else
        if (argc >= 2 && std::string(argv[1]) == "compilation") {
            std::cout << "Compilation successful." << std::endl;
        } else {
            bot.playGame();
        }
    #endif

    return 0;
}
