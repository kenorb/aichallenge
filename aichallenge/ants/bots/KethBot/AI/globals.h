#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

using namespace std;

inline int sign(int a) { return (a == 0) ? 0 : (a<0 ? -1 : 1); }

struct Bot;
extern Bot bot;

struct Map;
extern Map gameMap;

struct Logger;
extern Logger logger;

struct State;
extern State state;

#endif // GLOBALS_H_INCLUDED
