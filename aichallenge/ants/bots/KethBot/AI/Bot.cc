#include "Bot.h"
#include "Ant.h"
#include <iostream>
#include <fstream>
#include <sstream>


#ifdef __DEBUG
#include "Logger.h"
#endif

Bot::Bot()
{

};

void Bot::playGame()
{
    cin >> state;

    #ifdef __DEBUG
    logState(&state, true);
    #endif

    while(cin >> state)
    {
        makeMoves();
        endTurn();
    }
};

void Bot::firstMove()
{
    #ifdef __DEBUG
    state.debugLog << "ACTION: firstMove()" << endl;
    #endif

    new Ant();
}

void Bot::makeMoves()
{
    #ifdef __DEBUG
    logState(&state, false);
    #endif

    if (state.turn == 1) {
        firstMove();
    }

    //picks out moves for each ant
    for(int ant=0; ant<(int)state.ants.size(); ant++)
    {
        for(int d=0; d<TDIRECTIONS; d++)
        {
            Location loc = state.getLocation(state.ants[ant], d);

            if(state.grid[loc.row][loc.col] != '%')
            {
                state.makeMove(state.ants[ant], d);
                break;
            }
        }
    }
};

void Bot::endTurn()
{
    state.reset();
    state.turn++;

    cout << "go" << endl;
};
