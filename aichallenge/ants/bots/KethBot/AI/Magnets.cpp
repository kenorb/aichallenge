#include "Magnets.h"
#include "Bot.h"
#include "Location.h"

#include "const.h"
#include "globals.h"

void applyMagnetData(MagnetData& magnetData, char& magnetType, const Location& magnetLocation, Ant* ant)
{
    magnetData.ignoreDistance = false;
    magnetData.power = 0;

    if (magnetType == 'a') {
        magnetData.power = 50;
        magnetData.div = 50;
    } else
    if (magnetType == 'b') {
        if (bot.hasAggressiveMode()) {
            magnetData.power = -5;
            magnetData.div = 50;
        }
    }
}
