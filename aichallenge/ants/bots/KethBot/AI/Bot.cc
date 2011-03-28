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
    state.logger.logPreState(&state, true);
    #endif
    while(cin >> state)
    {
        onThink();
        endTurn();
    }
};

void Bot::firstMove()
{
    #ifdef __DEBUG
    state.logger.debugLog << "ACTION: firstMove()" << endl;
    #endif
}

void Bot::validateAnts()
{
    int i = 0;
    while (true) {
        i = 0;
        std::list<Ant*>::iterator iter_ant;
        for (iter_ant = state.structuralAnts.begin(); iter_ant != state.structuralAnts.end(); iter_ant++)
        {
            Ant* ant = (*iter_ant);
            if (state.grid[ant->loc.row][ant->loc.col] != 'a') {
                Ant* foundAnt = state.getAntAt(ant->loc);
                if (foundAnt) {
                    state.setAntAt(foundAnt->loc, NULL);
                    delete foundAnt;
                    i++;
                }
            }
            if (i > 0) break;
        }
        if (i == 0) break;
    }
}

void Bot::makeMoves()
{
    if (state.turn == 1) firstMove();

    for(int ant_id = 0; ant_id < (int)state.ants.size(); ant_id++)
    {
        Location loc = state.ants[ant_id];
        Ant* ant = state.getAntAt(loc);
        if (ant) {
            int nextMove = ant->getNextMove();
            if (nextMove != NO_MOVE) state.makeMove(loc, nextMove);
        } else {
            #ifdef __DEBUG
            state.logError("Structural ant at location not found");
            #endif
        }
    }
}

void Bot::onThink()
{
    #ifdef __DEBUG
    state.logger.debugLog << "[Turn #" << state.turn << "]:" << endl;
    state.logger.logPreState(&state, false);
    #endif

    validateAnts();
    makeMoves();

    #ifdef __DEBUG
    if ((int)state.ants.size() != (int)state.structuralAnts.size()) {
        state.logError("Number of ants given is not equal to structural ants");
    }

    state.logger.logPostState(&state);
    #endif


};

void Bot::endTurn()
{
    state.reset();
    state.turn++;

    cout << "go" << endl;
};
