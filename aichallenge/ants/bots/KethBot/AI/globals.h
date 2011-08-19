#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include <limits>
#include "vector2d.h"
#include <sys/time.h>

#ifdef __DEBUG
#include "Profiler.h"
#endif

using namespace std;

struct Bot;
extern Bot bot;

struct Map;
extern Map gameMap;

#ifdef __DEBUG
struct Logger;
extern Logger logger;
#endif

struct State;
extern State state;

#ifdef __DEBUG
extern Profiler profiler;
#endif

struct Location;

extern Location locNull;



inline int sign(int a) { return (a == 0) ? 0 : (a < 0 ? -1 : 1); }
double unix_time();
int IndexToRow(int row);
int IndexToCol(int col);
int LocToIndex(int row, int col);

#endif // GLOBALS_H_INCLUDED
