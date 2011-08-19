#include "State.h"
#include "Ant.h"
#include "Bot.h"
#include <time.h>

State::State()
{
    gameover = 0;
};

void State::setup()
{
    grid = vector<vector<char> >(rows, vector<char>(cols, '?'));
    ants_grid = vector<vector<int> >(rows, vector<int>(cols, 0));
    approxPlayers = 2;

    cout << "go" << endl;

    turn++;
};

void State::reset()
{
    ants.clear();
    enemyAnts.clear();
    for(int row=0; row<rows; row++)
    for(int col=0; col<cols; col++)
        if(grid[row][col] != '%' && grid[row][col] != '^') {
            grid[row][col] = '?';
        }
};

void State::updateFogOfWar()
{
    // TODO
    //int areaCells = 0;
    //areaCoverage = areaCells / (double)(rows * cols);

    int visibleCells = 0;
    int closeCells = 0;

    for(int row=0; row<rows; row++)
    for(int col=0; col<cols; col++)
    {
        if(grid[row][col] != '%' && grid[row][col] != 'a') {
            for(int ant_id = 0; ant_id < (int)ants.size(); ant_id++)
            {
                if (gameMap.distance(ants[ant_id], Loc(row, col)) < viewradius && grid[row][col] == '?') {
                    grid[row][col] = '.';
                }
            }
        }
    }

    for(int row=0; row<rows; row++)
    for(int col=0; col<cols; col++)
    {

        for(int ant_id = 0; ant_id < (int)ants.size(); ant_id++)
        {
            if (grid[row][col] != '?') {
                visibleCells++;
                break;
            }
    }
        }

    for(int row=0; row<rows; row++)
    for(int col=0; col<cols; col++)
    {
        for(int ant_id = 0; ant_id < (int)ants.size(); ant_id++)
        {
            if (gameMap.distance(ants[ant_id], Loc(row, col)) < 4) {
                if(grid[row][col] != '%' && grid[row][col] != 'a' && grid[row][col] != '*' && grid[row][col] != '^') {
                    grid[row][col] = 'o';
                }
                closeCells++;
                break;
            }
        }
    }

    visibilityCoverage = visibleCells / (double)(rows * cols);
    areaCoverage = closeCells / (double)(rows * cols);

}

void State::makeMove(const Location &loc, int direction)
{
    Location& locMoveTo = getLocation(loc, direction);
    #ifdef __DEBUG
    if ((direction < 0) || (direction > 3)) {
        logger.logWarning("Ant trying to move into incorrect direction");
    }

    if (grid[locMoveTo.row][locMoveTo.col] == '%' || grid[locMoveTo.row][locMoveTo.col] == 'a') {
        logger.logWarning("Ant trying to move inside a wall or another ant");
    }
    #endif

    grid[locMoveTo.row][locMoveTo.col] = 'a';
    grid[loc.row][loc.col] = '.';

    Ant* movingAnt = gameMap.getAntAt(loc);

    state.ants[movingAnt->temporaryId].row = locMoveTo.row;
    state.ants[movingAnt->temporaryId].col = locMoveTo.col;

    gameMap.onAntMoves(movingAnt, locMoveTo);

    cout << "o " << loc.row << " " << loc.col << " " << CDIRECTIONS[direction] << endl;

    movingAnt->onMove(locMoveTo);
};

//returns the new location from moving in a given direction with the edges wrapped
Location& State::getLocation(const Location &loc, int direction)
{
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

istream& operator>>(istream &is, State &state)
{
    int row, col, player;
    string inputType, junk;

    //finds out which turn it is
    while(is >> inputType)
    {
        if(inputType == "end")
        {
            bot.endGame();
            state.gameover = 1;
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
        while(is >> inputType)
        {
            if(inputType == "w") //water square
            {
                is >> row >> col;
                state.grid[row][col] = '%';

                Location& loc = Loc(row, col);
                gameMap.onWater(loc);
            }
            else if(inputType == "f") //food square
            {
                is >> row >> col;
                if (state.grid[row][col] != '^') state.grid[row][col] = '*';

                Location& loc = Loc(row, col);
                gameMap.onFood(loc);
            }
            else if(inputType == "a") //live ant square
            {
                is >> row >> col >> player;
                state.grid[row][col] = 'a' + player;

                Location& loc = Loc(row, col);

                if (player == 0) {
                    state.ants.push_back(Loc(row, col));
                } else {
                    if ((player + 1) > state.approxPlayers) {
                        state.approxPlayers = player + 1;
                    }
                    state.enemyAnts.push_back(Loc(row, col));
                }

                gameMap.onAnt(player, loc);
            }
            else if(inputType == "d") { //dead ant square
                is >> row >> col >> player;
                state.grid[row][col] = 'd';

                Location& loc = Loc(row, col);
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
                if(state.gameover)
                    is.setstate(std::ios::failbit);
                else
                    state.timer.start();
                break;
            }
            else //unknown line
                getline(is, junk);
        }
    }

    return is;
};
