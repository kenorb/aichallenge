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
#include "Map.h"
#include "Ant.h"

#include "Optimizer.h"
#include "Damage.h"

#ifdef __DEBUG
#include "Logger.h"
#endif

class Ant;

struct CallbackLocData;

void _updateLocation(const Location& loc, CallbackLocData data);
void _addFriendlyDamage(const Location& loc, CallbackLocData data);
void _addEnemyDamage(const Location& loc, CallbackLocData data);
void _resetDamage(const Location& loc, CallbackLocData data);

struct LocationRef;

struct State
{
    int rows, cols, turn, turns, players;
    double attackradius, spawnradius, viewradius;
    int approxPlayers;

    double loadtime, turntime;
    vector<double> scores;
    bool gameover;

    double visibilityCoverage;
    double areaCoverage;
    double lastGo;

    void sortDamageGrid();
    void updateLocations();

    int visibleCells, closeCells, updatedCells;

    #ifdef __DEBUG
    int lastVisibleCells;
    #endif

    bool isEnemy(char sq);

    std::stringstream moves;

    char** grid;
    int** ants_grid;
    LocationCache** locCache;
    bool** grid_visible;

    vector<vector<bool> > grid_needsUpdate;
    vector<vector<int> > grid_prediction;

    vector<Location> ants;
    std::list<int> priorityAnts;
    vector<Location> enemyAnts;
    vector<const Location*> foodLocations;
    std::list<Ant*> structuralAnts;


    Damage* gridDamage;
    LocationRef** gridFriendlyAnts;
    const Location** magnets;

    int magnetsCount;
    //const LocationReference*** gridEnemyAnts;


    Timer timer;

    State();

    void setup();

    void beforeInput();
    void afterInput();

    void resetLocation(const int row, const int col);
    //void updateDamage(const int row, const int col, const void* sender /* = NULL*/);
    void updateLocation(const Location& loc, CallbackLocData data);
    void updateLocation(const int row, const int col, CallbackLocData data);
    void updateFogOfWar();

    void reset();

    void makeMove(const Location &loc, int direction);
    double timeLeft();

    const Location& getLocation(const Location &loc, int direction) const;
};

ostream& operator<<(ostream &os, const State &state);
istream& operator>>(istream &is, State &state);


#endif //STATE_H_
