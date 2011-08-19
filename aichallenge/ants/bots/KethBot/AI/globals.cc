#include "Bot.h"

#ifdef __DEBUG
#include "Profiler.h"
#endif

#ifdef __DEBUG
Logger logger;
#endif

Bot bot;
State state;
Map gameMap;
Location locNull;

#ifdef __DEBUG
Profiler profiler;
#endif


double unix_time()
{
    timeval timer;
    gettimeofday(&timer, NULL);
    return timer.tv_sec+(timer.tv_usec/1000000.0);
}

int IndexToRow(int row) {
    return floor(row / state.cols);
}

int IndexToCol(int col) {
    return col % state.cols;
}

int LocToIndex(int row, int col) {
    return row * state.cols + col;
}
