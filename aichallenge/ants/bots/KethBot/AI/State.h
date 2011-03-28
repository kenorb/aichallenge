#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <list>

#include "Timer.h"
#include "Bug.h"
#include "Location.h"
#include "Ant.h"
#include "const.h"

#ifdef __DEBUG
#include "Logger.h"
#endif

using namespace std;

/*
    constants
*/


/*
    struct to store current state information
*/
struct State
{
    /*
        Variables
    */

    int rows, cols,
        turn, turns,
        players,
        attackradius, spawnradius, viewradius;
    double loadtime, turntime;
    vector<double> scores;
    bool gameover;

    vector<vector<char> > grid;
    vector<vector<int> > ants_grid;
    vector<Location> ants;
    std::list<Ant*> structuralAnts;

    #ifdef __DEBUG
    Logger logger;
    #endif
    Timer timer;



    int errors;
    int warnings;

    /*
        Functions
    */
    State();
    ~State();

    void setup();
    void reset();

    void logError(std::string error);
    void logWarning(std::string warning);

    void makeMove(const Location &loc, int direction);
    Ant* getAntAt(const Location &loc);
    Ant* setAntAt(const Location &loc, Ant* ant);

    double distance(const Location &loc1, const Location &loc2);
    Location getLocation(const Location &startLoc, int direction);
};

ostream& operator<<(ostream &os, const State &state);
istream& operator>>(istream &is, State &state);

#endif //STATE_H_
