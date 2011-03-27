#include "Bot.h"
#include <iostream>
#include <fstream>
#include <sstream>

// viewradius = 9


#ifdef __DEBUG
#include <json/json.h>
#endif

Bot::Bot()
{

};

//plays a single game of Ants.
void Bot::playGame()
{
    //reads the game parameters
    cin >> state;

    //continues making moves while the game is not over
    while(cin >> state)
    {
        makeMoves();
        endTurn();
    }

};

void Bot::debugData()
{
    #ifdef __DEBUG

    char key[11];
    std::stringstream os;

    sprintf(key, "%03i", state.turn);
    debugNode[key]["ants_alive"] = state.ants.size();

    os << state;
    debugNode[key]["map_visible"] = os.str();
    os.str("");

    os << (float)state.timer.getTime() << "ms";
    debugNode[key]["time_taken"] = os.str();
    os.str("");

    std::string outputConfig = debugOutput.write(debugNode);
    state.bug.setText(outputConfig);

    #endif
}

//makes the bots moves for the turn
void Bot::makeMoves()
{
    debugData();

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

//finishes the turn
void Bot::endTurn()
{
    state.reset();
    state.turn++;

    cout << "go" << endl;
};
