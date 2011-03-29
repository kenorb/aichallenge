#include "Location.h"
#include "Ant.h"
#include "Bot.h"

bool Location::think()
{
    if (Ant* ant = gameMap.getAntAt(*this)) {
        ant->onThink();
        return true;
    }
    return false;
}
