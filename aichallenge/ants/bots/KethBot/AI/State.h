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

struct State
{
    int rows, cols, turn, turns, players, attackradius, spawnradius, viewradius;
    double loadtime, turntime;
    vector<double> scores;
    bool gameover;

    double visibilityCoverage;
    double areaCoverage;

    vector<vector<char> > grid;
    vector<vector<int> > ants_grid;
    vector<Location> ants;
    vector<Location> enemyAnts;
    std::list<Ant*> structuralAnts;

    Timer timer;

    State();

    void setup();
    void updateFogOfWar();

    void reset();

    void makeMove(const Location &loc, int direction);

    Location getLocation(const Location &startLoc, int direction);
};

ostream& operator<<(ostream &os, const State &state);
istream& operator>>(istream &is, State &state);

#endif //STATE_H_
