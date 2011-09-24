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
        magnetData.power = 5;
        magnetData.div = 15;

        if (magnetData.distance > state.viewradius + 3) {
            magnetData.power = 0;

            LocationCache& cache = LocCache(magnetLocation);

            if (cache.nearestAnt[FRIEND][0].distance > state.viewradius) {
                if (ant && ant->isEnemyInRange(state.viewradius)) {
                    magnetData.power = 0;
                } else {
                    magnetData.power = -magnetData.power;
                }
            }
        }
    } else
    if (magnetType == '*') {
        magnetData.power = -100;
        magnetData.div = 15;

        LocationCache& cache = LocCache(magnetLocation);

        if (ant) {
            LocationRef& _friend = LocFindAnt(magnetLocation, FRIEND, FIRST);
            if (didFind(_friend)) {
                Ant* friendAnt = _friend.location->getAnt();
                #ifdef __ASSERT
                if (!friendAnt) {
                    logger.logError("getAnt(findAnt) = NULL");
                    logger.debugLog << "magnetLocation: " << LocationToString(magnetLocation) << std::endl;
                    logger.debugLog << "friendAnt should be at " << LocationToString(*_friend.location) << std::endl;
                }
                #endif

                bool collisionLine = magnetLocation.collisionLine(AntLoc(friendAnt));

                if (friendAnt && friendAnt != ant && !collisionLine) {
                    magnetData.power = 0;
                }
            }

            if (magnetData.power != 0) {
                LocationRef& enemy = LocFindAnt(magnetLocation, ENEMY, FIRST);
                if (didFind(enemy)) {
                    if (enemy.distance < magnetData.distance && !enemy.location->collisionLine(magnetLocation)) {
                        magnetData.power = 0;
                    }
                }
            }
        }

        // use heightmap of cost to pick direction??

    } else
    if (magnetType == 'b') {
        bool attack = false;
        if (bot.hasAggressiveMode()) attack = true;

        if (attack) {
            magnetData.power = -5;
            magnetData.div = 50;
        } else {
            if (LocFindAnt(magnetLocation, FRIEND, FIRST).distance > LocFindAnt(magnetLocation, ENEMY, FIRST).distance) {
                magnetData.power = 1;
                magnetData.div = 15;
            }
        }
    }
}
