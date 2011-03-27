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
    state.changes.clear();
    cin >> state;

    #ifdef __DEBUG
    state.logger.logState(&state, true);
    #endif
    while(cin >> state)
    {
        makeMoves();
        endTurn();
        state.changes.clear();
    }
};

void Bot::firstMove()
{
    #ifdef __DEBUG
    state.debugLog << "ACTION: firstMove()" << endl;
    #endif

    //new Ant();
}

void Bot::makeMoves()
{
    #ifdef __DEBUG
    state.logger.logState(&state, false);
    #endif

    if (state.turn == 1) {
        firstMove();
    }

    state.debugLog << "[#" << (int)state.turn << "] changes: " << state.changes.size() << endl;
    for(int change_id=0; change_id<(int)state.changes.size(); change_id++)
    {
        Changes change = state.changes[change_id];

        if (change.type == "newant") {
            if (!state.getAntAt(change.loc1)) {
                state.debugLog << "  - change " << (int)change_id << ": " << change.type << " ( " << change.loc1.str() << "  -  " << change.loc2.str() << ")" << endl;
                state.setAntAt(change.loc1, new Ant(state, change.loc1));
            }
        }
    }


    for(int ant_id=0; ant_id<(int)state.ants.size(); ant_id++)
    {
        Location loc = state.ants[ant_id];
        for(int d=0; d<TDIRECTIONS; d++)
        {
            Location locMoveTo = state.getLocation(loc, d);
            if(state.grid[locMoveTo.row][locMoveTo.col] != '%')
            {
                //state.makeMove(loc, d);
                break;
            }
        }
    }

    state.debugLog << "[#" << (int)state.turn << "] changes: " << state.changes.size() << endl;
    for(int change_id=0; change_id<(int)state.changes.size(); change_id++)
    {
        Changes change = state.changes[change_id];

        if (change.type == "antmoved") {
            state.debugLog << "  - change " << (int)change_id << ": " << change.type << " ( " << change.loc1.str() << "  -  " << change.loc2.str() << ")" << endl;
            Ant* temp = state.getAntAt(change.loc2);
            state.setAntAt(change.loc2, state.getAntAt(change.loc1));
            state.setAntAt(change.loc1, temp);
        } else
        if (change.type == "antdies") {

        }
    }

    state.changes.clear();


};

void Bot::endTurn()
{
    state.reset();
    state.turn++;

    cout << "go" << endl;
};
