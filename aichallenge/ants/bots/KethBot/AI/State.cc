#include <time.h>
#include <algorithm>
#include <vector>

#include "State.h"
#include "Ant.h"
#include "Sorter.h"
#include "Bot.h"
#include "Optimizer.h"

#include "globals.h"
#include "relativeLocation.h"

State::State()
{
    gameover = 0;
};

void State::setup()
{
    grid = vector<vector<char> >(rows, vector<char>(cols, '?'));
    ants_grid = vector<vector<int> >(rows, vector<int>(cols, 0));
    grid_needsUpdate = vector<vector<bool> >(rows, vector<bool>(cols, false));
    grid_damage = vector<vector<Damage> >(rows, vector<Damage>(cols, Damage()));
    grid_prediction = vector<vector<int> >(rows, vector<int>(cols, 0));
    grid_visible = vector<vector<bool> >(rows, vector<bool>(cols, false));

    approxPlayers = 2;
    visibleCells = 0;

    cout << "go" << endl;

    turn++;
};

void State::reset()
{
    ants.clear();
    enemyAnts.clear();
    for(int row=0; row<rows; row++)
    for(int col=0; col<cols; col++)
        resetLocation(row, col);
};

bool State::isEnemy(char sq) {
    return sq == 'b' || sq == 'c' || sq == 'd' || sq == 'e' || sq == 'f' || sq == 'g' || sq == 'h';
}

void State::resetLocation(const int row, const int col)
{
    if(grid[row][col] != '%' && grid[row][col] != '^') {
        grid[row][col] = '?';
    }
}

void State::updateLocation(const Location& loc, CallbackLocData data) {
    state.updateLocation(loc.row, loc.col, data);
}

void State::updateLocation(const int row, const int col, CallbackLocData data)
{
    const Location& loc = Loc(row, col);

    bool _isFog = grid[row][col] == '?';
    bool _isWall = grid[row][col] == '%';
    bool _isMe = grid[row][col] == 'a';
    bool _isEnemy = grid[row][col] == 'b';
    bool _emptyCell = !_isWall && grid[row][col] != 'a' && grid[row][col] != 'b' && grid[row][col] != '*' && grid[row][col] != '^';
    bool flagVisible = false;
    bool flagClose = false;

    if (data.sender) {
        if (Ant* ant = (Ant*)data.sender) {
            const double* distance = &data.relLoc->distance;

            #ifdef __ASSERT
            double distance2 = distance_fast(ant->getLocation().row, ant->getLocation().col, row, col);
            if (*distance != distance2) logger.logError("*distance != distance2");
            #endif

            if (_isFog && *distance <= viewradius) {
                _isFog = false;
                _emptyCell = true;
                grid[row][col] = '.';
            }

            if (_isEnemy) {
                if (*distance <= state.viewradius) {
                    ant->seenEnemy = true;
                }
            }

            if (_emptyCell && *distance < 5 && grid[row][col] != 'o') {
                grid[row][col] = 'o';
                closeCells++;
            }
        }
    }

    if (_isFog) {
        if (grid_visible[row][col] == true) {
            loc.onDisappear();
        }
        grid_visible[row][col] = false;
    } else {
        if (grid_visible[row][col] == false) {
            loc.onAppear();
            updatedCells++;
            visibleCells++;
        }
        grid_visible[row][col] = true;
    }

    //updateDamage(row, col);

/*
    for(int ant_id = 0; ant_id < (int)ants.size(); ant_id++)
    {
        double distance = distance_fast(ants[ant_id].row, ants[ant_id].col, row, col);

        if (_isFog && distance < viewradius) {
            _isFog = false;
            _emptyCell = true;
            grid[row][col] = '.';
        }

        if (_isEnemy) {
            if (distance <= state.viewradius) {
                Ant* ant = getAnt(ant_id);
                if (ant) ant->seenEnemy = true;
            }
        }

        if (_emptyCell && distance < 5 && grid[row][col] != 'o') {
            grid[row][col] = 'o';
            closeCells++;
        }
    }
    */
}

void _updateLocation(const Location& loc, CallbackLocData data)
{
    state.updateLocation(loc.row, loc.col, data);
}

void State::updateLocations()
{
    for(int ant_id = 0; ant_id < (int)ants.size(); ant_id++) {
        gameMap.callbackArea(Loc(ants[ant_id].row, ants[ant_id].col), state.viewradius, &_updateLocation, getAntById(ant_id));
    }
}


void _addEnemyPrediction(const Location& loc, CallbackLocData data)
{
    state.grid_prediction[loc.row][loc.col]++;
}

void _addEnemyDamage(const Location& loc, CallbackLocData data)
{
    //logger.debugLog << " +1 enemy at " << LocationToString(loc) << std::endl;

    const Location* senderLocation = (Location*)data.sender;

    if (state.grid_damage[loc.row][loc.col].enemy == 0) {
        state.grid_damage[loc.row][loc.col].enemy = 1;
    } else {
        for (int i = 0; i < state.grid_damage[loc.row][loc.col].enemyAnts.size(); i++) {
            const Location& enemyAnt = *state.grid_damage[loc.row][loc.col].enemyAnts[i];
            if (enemyAnt.row == senderLocation->row && enemyAnt.col == senderLocation->col) {
                return;
            }
        }
        state.grid_damage[loc.row][loc.col].enemy++;
    } // 578



    if (senderLocation) {
        state.grid_damage[loc.row][loc.col].enemyAnts.push_back(senderLocation);
        double dist = distance_fast(loc.row, loc.col, senderLocation->row, senderLocation->col);

        if (state.grid_damage[loc.row][loc.col].enemyDistance >= 0) {
            state.grid_damage[loc.row][loc.col].enemyDistance = min(state.grid_damage[loc.row][loc.col].enemyDistance, dist);
        } else {
            state.grid_damage[loc.row][loc.col].enemyDistance = dist;
        }

        if (state.grid_damage[loc.row][loc.col].prediction == false) {
            if (dist > state.attackradius) state.grid_damage[loc.row][loc.col].prediction = true;
        }
    }
}

void _resetDamage(const Location& loc, CallbackLocData data)
{
    state.grid_damage[loc.row][loc.col].friendly = 0;
    state.grid_damage[loc.row][loc.col].enemy = 0;
    state.grid_damage[loc.row][loc.col].attacked = 0;
    state.grid_damage[loc.row][loc.col].friendlyDistance = -1;
    state.grid_damage[loc.row][loc.col].enemyDistance = -1;
    state.grid_damage[loc.row][loc.col].sideDamage[0] = 0;
    state.grid_damage[loc.row][loc.col].sideDamage[1] = 0;
    state.grid_damage[loc.row][loc.col].sideDamage[2] = 0;
    state.grid_damage[loc.row][loc.col].sideDamage[3] = 0;
    state.grid_damage[loc.row][loc.col].friendlyAnts.clear();
    state.grid_damage[loc.row][loc.col].enemyAnts.clear();
}

void _deleteFriendlyDamage(const Location& loc, CallbackLocData data)
{
    vector<const Location*>& friendlyAnts = state.grid_damage[loc.row][loc.col].friendlyAnts;
    friendlyAnts.erase(std::remove(friendlyAnts.begin(), friendlyAnts.end(), data.senderLocation), friendlyAnts.end());
    state.grid_damage[loc.row][loc.col].friendly--;

    #ifdef __ASSERT
    if (friendlyAnts.size() != state.grid_damage[loc.row][loc.col].friendly) {
        logger.logError("friendlyAnts.size() != state.grid_damage[loc.row][loc.col].friendly");
        logger.debugLog << "DEBUG: " << friendlyAnts.size() << " != " << state.grid_damage[loc.row][loc.col].friendly << std::endl;
    }
    #endif
}

void _addFriendlyDamage(const Location& loc, CallbackLocData data)
{
    //logger.debugLog << " +1 self at " << LocationToString(loc) << std::endl;

    state.grid_damage[loc.row][loc.col].friendly++;

    if (data.senderLocation) {
        state.grid_damage[loc.row][loc.col].friendlyAnts.push_back(data.senderLocation);
        double dist = distance_fast(loc.row, loc.col, data.senderLocation->row, data.senderLocation->col);

        if (state.grid_damage[loc.row][loc.col].friendlyDistance >= 0) {
            state.grid_damage[loc.row][loc.col].friendlyDistance = min(state.grid_damage[loc.row][loc.col].friendlyDistance, dist);
        } else {
            state.grid_damage[loc.row][loc.col].friendlyDistance = dist;
        }
    }
}

/*
void State::updateDamage(const int row, const int col, const void* sender)
{
    if (grid_needsUpdate[row][col]) {
        grid_needsUpdate[row][col] = false;

        bool _isMe = grid[row][col] == 'a';
        bool _isEnemy = grid[row][col] == 'b';

        if (grid[row][col] == '*' || grid[row][col] == '*') {
            const Location& nearestEnemy = Loc(row, col).nearestEnemyAnt();
            if (nearestEnemy.isValid() && distance_fast(row, col, nearestEnemy.row, nearestEnemy.col) <= (state.spawnradius + 1)) {
                _isEnemy = true;
            }
        }

        if (_isEnemy) {
            gameMap.callbackArea(Loc(row, col), sqrt(11), &_addEnemyDamage, &Loc(row, col));
        } else
        if (_isMe) {
            gameMap.callbackArea(Loc(row, col), state.attackradius, &_addFriendlyDamage, &Loc(row, col));
        }
    }
}*/

void State::sortDamageGrid()
{
    #ifdef __DEBUG
    profiler.beginThinkTime(TT_MAP);
    #endif

    //const Location& startLoc = Loc(0, 0);
    //const Location& endLoc = Loc(state.rows-1, state.cols-1);
/*
    for(int row=startLoc.row; row<=endLoc.row; row++)
    for(int col=startLoc.col; col<=endLoc.col; col++) {
        gameMap.callbackArea(Loc(row, col), state.attackradius, &_resetDamage, &Loc(row, col));
    }

    for(int row=startLoc.row; row<=endLoc.row; row++)
    for(int col=startLoc.col; col<=endLoc.col; col++) {
        bool _isMe = grid[row][col] == 'a';
        bool _isEnemy = grid[row][col] == 'b';

        if (grid[row][col] == '*' || grid[row][col] == '*') {
            const Location& nearestEnemy = Loc(row, col).nearestEnemyAnt();
            if (nearestEnemy.isValid() && distance_fast(row, col, nearestEnemy.row, nearestEnemy.col) <= (state.spawnradius + 1)) {
                _isEnemy = true;
            }
        }

        if (_isMe) {
            gameMap.callbackArea(Loc(row, col), state.attackradius, &_addFriendlyDamage, &Loc(row, col));
        } else
        if (_isEnemy) {
            //gameMap.callbackArea(Loc(row, col), sqrt(sqr(state.attackradius) + 5), &_addEnemyDamage, &Loc(row, col));
            gameMap.callbackArea(Loc(row, col), state.attackradius, &_addEnemyDamage, &Loc(row, col));

            gameMap.callbackArea(Loc(row-1, col), state.attackradius, &_addEnemyDamage, &Loc(row, col));
            gameMap.callbackArea(Loc(row+1, col), state.attackradius, &_addEnemyDamage, &Loc(row, col));
            gameMap.callbackArea(Loc(row, col-1), state.attackradius, &_addEnemyDamage, &Loc(row, col));
            gameMap.callbackArea(Loc(row, col+1), state.attackradius, &_addEnemyDamage, &Loc(row, col));
        }
    }*/

    for(int row=0; row<state.rows; row++)
    for(int col=0; col<state.cols; col++) {
        #ifdef __ASSERT
        if (grid_damage[row][col].friendly != grid_damage[row][col].friendlyAnts.size()) logger.logError("grid_damage[row][col].friendly != grid_damage[row][col].friendlyAnts.size()");
        if (grid_damage[row][col].enemy != grid_damage[row][col].enemyAnts.size()) logger.logError("grid_damage[row][col].enemy != grid_damage[row][col].enemyAnts.size()");
        #endif

        if (grid_damage[row][col].friendly >= 2) {
            std::sort(grid_damage[row][col].friendlyAnts.begin(), grid_damage[row][col].friendlyAnts.end(), sortLocationsFrom(Loc(row, col)));
        }

        if (grid_damage[row][col].enemy >= 2) {
            std::sort(grid_damage[row][col].enemyAnts.begin(), grid_damage[row][col].enemyAnts.end(), sortLocationsFrom(Loc(row, col)));
        }
    }

/*
    for(int ant_id = 0; ant_id < (int)ants.size(); ant_id++) {
        gameMap.callbackArea(Loc(ants[ant_id].row, ants[ant_id].col), state.viewradius, &_updateDamage);
    }*/

    #ifdef __DEBUG
    profiler.endThinkTime(TT_MAP);
    #endif
}

void State::updateFogOfWar()
{
    updatedCells = 0;
    closeCells = 0;

    state.updateLocations();

    #ifdef __ASSERT
    int j = 0;
    for(int row=0; row<rows; row++)
    for(int col=0; col<cols; col++)
    {
        if (grid_visible[row][col]) j++;
        for(int ant_id = 0; ant_id < (int)ants.size(); ant_id++)
        {
            double distance = distance_fast(ants[ant_id].row, ants[ant_id].col, row, col);

            if (distance <= viewradius && !grid_visible[row][col]) {
                logger.logError("distance <= viewradius && !grid_visible[row][col])");
            }
        }
    }

    if (visibleCells != j) {
        logger.logError("j != visibleCells");
        logger.debugLog << "DEBUG: j = " << j << ", visibleCells: " << visibleCells << std::endl;
    }


    if(visibleCells-lastVisibleCells != updatedCells){
        logger.logError("visibleCells-lastVisibleCells != visibleCells");
        logger.debugLog << "DEBUG: visibleCells-lastVisibleCells = " << visibleCells-lastVisibleCells << ", visibleCells: " << visibleCells << std::endl;
    }


    if (visibleCells > (rows * cols)) {
        logger.logError("visibleCells > (rows * cols)");
        logger.debugLog << "DEBUG: visibleCells = " << visibleCells << ", rows*cols: " << rows*cols << std::endl;
    }
    #endif

    #ifdef __DEBUG
    logger.debugLog << "Visible cells: " << visibleCells << ", Updated cells: " << updatedCells << std::endl;
    #endif


    visibilityCoverage = (visibleCells / (double)(rows * cols));
    areaCoverage = closeCells / (double)(rows * cols);

    #ifdef __DEBUG
    lastVisibleCells = visibleCells;
    #endif
}

void State::makeMove(const Location &loc, int direction)
{
    const Location& locMoveTo = getLocation(loc, direction);

    #ifdef __ASSERT
    if ((direction < 0) || (direction > 3)) {
        logger.logError("Ant trying to move into incorrect direction");
    }

    if (grid[locMoveTo.row][locMoveTo.col] == '%' || grid[locMoveTo.row][locMoveTo.col] == 'a') {
        logger.logError("Ant trying to move inside a wall or another ant");
    }
    #endif

    gameMap.callbackArea(Loc(loc.row, loc.col), state.attackradius, &_deleteFriendlyDamage, &Loc(loc.row, loc.col));

    grid[locMoveTo.row][locMoveTo.col] = 'a';
    grid[loc.row][loc.col] = '.';

    Ant* movingAnt = gameMap.getAntAt(loc);

    state.ants[movingAnt->temporaryId].row = locMoveTo.row;
    state.ants[movingAnt->temporaryId].col = locMoveTo.col;

    gameMap.onAntMoves(movingAnt, locMoveTo);

    cout << "o " << loc.row << " " << loc.col << " " << CDIRECTIONS[direction] << endl;

    movingAnt->onMove(locMoveTo);

    gameMap.callbackArea(Loc(locMoveTo.row, locMoveTo.col), state.attackradius, &_addFriendlyDamage, &Loc(locMoveTo.row, locMoveTo.col));

    state.sortDamageGrid();
};

//returns the new location from moving in a given direction with the edges wrapped
const Location& State::getLocation(const Location &loc, int direction) const
{
    if (direction <= NO_MOVE) return loc;

    return Loc( (loc.row + DIRECTIONS[direction][0] + rows) % rows,
                     (loc.col + DIRECTIONS[direction][1] + cols) % cols );
};

ostream& operator<<(ostream &os, const State &state)
{
    for(int row=0; row<state.rows; row++)
    {
        for(int col=0; col<state.cols; col++)
            os << state.grid[row][col];
        os << "<br>"; // removed endl
    }

    return os;
};

double State::timeLeft() {
    return state.turntime - state.timer.getTime();
}

void State::beforeInput() {
    for(int row = 0; row < state.rows; row++)
    for(int col = 0; col < state.cols; col++) {
        _resetDamage(Loc(row, col), CallbackLocData());
    }

    foodLocations.clear();
}

void State::afterInput() {
    for(int row = 0; row < state.rows; row++)
    for(int col = 0; col < state.cols; col++) {
        if (state.grid[row][col] == '*' || state.grid[row][col] == '^') {
            bool foodExists = false;

            for (int i = 0; i < foodLocations.size(); i++) {
                const Location& foodLocation = *foodLocations[i];
                if (foodLocation.row == row && foodLocation.col == col) {
                    foodExists = true;
                    break;
                }
            }

            if (!foodExists) {
                state.grid[row][col] = '?';
            }
        }
    }

}


istream& operator>>(istream &is, State &state)
{
    #ifdef __DEBUG
    profiler.beginThinkTime(TT_INPUT);
    #endif

    int row, col, player;
    string inputType, junk;



    //finds out which turn it is
    while(is >> inputType)
    {
        if(inputType == "end")
        {
            #ifdef __DEBUG
            profiler.endThinkTime(TT_INPUT);
            #endif

            bot.endGame();
            state.gameover = 1;
            exit(1);
            break;
        }
        else if(inputType == "turn")
        {
            is >> state.turn;
            break;
        }
        else //unknown line
            getline(is, junk);
    }
    #ifdef __DEBUG
    if (!state.gameover) {
        logger.debugLog << "[TURN #" << state.turn << "]:" << endl;
    }
    #endif
    if(state.turn == 0)
    {
        //reads game parameters
        while(is >> inputType)
        {
            if(inputType == "loadtime")
                is >> state.loadtime;
            else if(inputType == "turntime")
                is >> state.turntime;
            else if(inputType == "rows")
                is >> state.rows;
            else if(inputType == "cols")
                is >> state.cols;
            else if(inputType == "turns")
                is >> state.turns;
            else if(inputType == "viewradius2")
            {
                is >> state.viewradius;
                state.viewradius = sqrt(state.viewradius);
            }
            else if(inputType == "attackradius2")
            {
                is >> state.attackradius;
                state.attackradius = sqrt(state.attackradius);
            }
            else if(inputType == "spawnradius2")
            {
                is >> state.spawnradius;
                state.spawnradius = sqrt(state.spawnradius);
            }
            else if(inputType == "ready") //end of parameter input
            {
                gameMap.onInit();
                state.setup();
                break;
            }
            else    //unknown line
                getline(is, junk);
        }
    }
    else
    {
        //reads information about the current turn
        state.beforeInput();

        while(is >> inputType)
        {
            if(inputType == "w") //water square
            {
                is >> row >> col;
                state.grid[row][col] = '%';

                const Location& loc = Loc(row, col);
                gameMap.onWater(loc);
            }
            else if(inputType == "f") //food square
            {
                is >> row >> col;
                if (state.grid[row][col] != '^') state.grid[row][col] = '*';

                const Location& loc = Loc(row, col);

                state.foodLocations.push_back(&Loc(row, col));
                gameMap.onFood(loc);
            }
            else if(inputType == "a") //live ant square
            {
                is >> row >> col >> player;
                //state.grid[row][col] = 'a' + player;

                const Location& loc = Loc(row, col);

                if (player == 0) {
                    state.grid[row][col] = 'a';
                    state.ants.push_back(Loc(row, col));

                    gameMap.callbackArea(Loc(row, col), state.attackradius, &_addFriendlyDamage, &Loc(row, col));
                } else {
                    state.grid[row][col] = 'b';
                    if ((player + 1) > state.approxPlayers) {
                        state.approxPlayers = player + 1;
                        // keep track of which players are being aggressive?
                    }
                    state.enemyAnts.push_back(Loc(row, col));

                    //gameMap.callbackArea(Loc(row, col), 1, &_addEnemyPrediction, &Loc(row, col));
/*
                    gameMap.callbackArea(Loc(row, col), state.attackradius, &_addEnemyDamage, &Loc(row, col));
                    gameMap.callbackArea(Loc(row-1, col), state.attackradius, &_addEnemyDamage, &Loc(row, col));
                    gameMap.callbackArea(Loc(row+1, col), state.attackradius, &_addEnemyDamage, &Loc(row, col));
                    gameMap.callbackArea(Loc(row, col-1), state.attackradius, &_addEnemyDamage, &Loc(row, col));
                    gameMap.callbackArea(Loc(row, col+1), state.attackradius, &_addEnemyDamage, &Loc(row, col));
*/
                    gameMap.callbackArea(Loc(row, col), sqrt(sqr(state.attackradius) + 5), &_addEnemyDamage, &Loc(row, col));
                }

                gameMap.onAnt(player, loc);
            }
            else if(inputType == "d") { //dead ant square
                is >> row >> col >> player;
                state.grid[row][col] = 'd';

                const Location& loc = Loc(row, col);
                gameMap.onDeadAnt(player, loc);

            }
            else if(inputType == "players") //player information
                is >> state.players;
            else if(inputType == "scores") //score information
            {
                state.scores = vector<double>(state.players, 0.0);
                for(int p=0; p<state.players; p++)
                    is >> state.scores[p];
            }
            else if(inputType == "go") //end of turn input
            {
                state.lastGo = unix_time();
                if(state.gameover)
                    is.setstate(std::ios::failbit);
                else
                    state.timer.start();
                break;
            }
            else //unknown line
                getline(is, junk);
        }
        state.afterInput();
    }

    #ifdef __DEBUG
    profiler.endThinkTime(TT_INPUT);
    #endif

    return is;
};
