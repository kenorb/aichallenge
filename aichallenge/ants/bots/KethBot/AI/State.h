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

#include "Timer.h"
#include "Bug.h"
#include "Location.h"
#include "Ant.h"
#include "Logger.h"

using namespace std;

/*
    constants
*/
const int TDIRECTIONS = 4;
const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };      //{N, E, S, W}

struct Changes
{
    Location loc1;
    Location loc2;
    string type;

    Changes()
    {
        type = "???";
    };

    Changes(const Location &_loc1, const Location &_loc2, string _type)
    {
        loc1.col = _loc1.col;
        loc1.row = _loc1.row;

        loc2.col = _loc2.col;
        loc2.row = _loc2.row;

        type = _type;
    };
};

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
    vector<Changes> changes;

    Logger logger;
    Timer timer;
    Bug jsonLog;
    Bug debugLog;

    /*
        Functions
    */
    State();
    ~State();

    void setup();
    void reset();

    void makeMove(const Location &loc, int direction);
    Ant* getAntAt(const Location &loc);
    Ant* setAntAt(const Location &loc, Ant* ant);

    double distance(const Location &loc1, const Location &loc2);
    Location getLocation(const Location &startLoc, int direction);
};

ostream& operator<<(ostream &os, const State &state);
istream& operator>>(istream &is, State &state);

#endif //STATE_H_
