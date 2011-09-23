#include "Bot.h"
#include "State.h"
#include "globals.h"

#ifdef __DEBUG
#include "Profiler.h"
#endif

#ifdef __DEBUG
Logger logger;
int codeDepth;
#endif

Bot bot;
State state;
Map gameMap;
Location* locNull;
Optimizer optimizer;

#ifdef __DEBUG
Profiler profiler;
#endif


double unix_time()
{
    timeval timer;
    gettimeofday(&timer, NULL);
    return timer.tv_sec+(timer.tv_usec/1000000.0);
}

int IndexToRow(const int row) {
    return floor(row / state.cols);
}

int IndexToCol(const int col) {
    return col % state.cols;
}

int isPowerOfTwo(unsigned int x)
{
    while (((x & 1) == 0) && x > 1) /* While x is even and > 1 */
    x >>= 1;
    return (x == 1);
}
