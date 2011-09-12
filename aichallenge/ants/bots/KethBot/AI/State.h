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

#include "globals.h"
#include "Optimizer.h"


#ifdef __DEBUG
#include "Logger.h"
#endif

struct CallbackLocData;

void _updateLocation(const Location& loc, CallbackLocData data);
void _addFriendlyDamage(const Location& loc, CallbackLocData data);
void _addEnemyDamage(const Location& loc, CallbackLocData data);
void _resetDamage(const Location& loc, CallbackLocData data);

struct Damage
{
    Damage() { friendly = enemy = attacked = 0; enemyDistance = -1; friendlyDistance = -1; prediction = false; };

    int friendly, enemy;

    vector<const Location*> friendlyAnts;
    vector<const Location*> enemyAnts;

    int sideDamage[4];
    int countSideDamage() {
        int ret = 0;
        for (int i = 0; i < 4; i++) ret += sideDamage[i];
        return ret;
    }
    void increaseSideDamage(int side) { sideDamage[side]++; }

    double friendlyDistance, enemyDistance;
    int attacked;
    bool prediction;
};

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

    vector<vector<char> > grid;
    vector<vector<bool> > grid_needsUpdate;
    vector<vector<Damage> > grid_damage;
    vector<vector<int> > grid_prediction;
    vector<vector<bool> > grid_visible;
    vector<vector<int> > ants_grid;
    vector<Location> ants;
    std::list<int> priorityAnts;
    vector<Location> enemyAnts;
    vector<const Location*> foodLocations;
    std::list<Ant*> structuralAnts;

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
