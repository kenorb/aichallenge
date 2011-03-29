#include "Location.h"
#include "Ant.h"
#include "Bot.h"

bool Location::think()
{
    if (Ant* ant = bot.map.getAntAt(*this)) {
        ant->onThink();
        return true;
    }
    return false;
}
