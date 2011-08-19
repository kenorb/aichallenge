#include "Logger.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "AI/Bot.h"

std::string LocationToString(Location& loc)
{
    std::stringstream s;
    s << "[POS " << loc.row << " x " << loc.col << "]";
    return s.str();
}

Logger::Logger()
{
    errors = 0;
    warnings = 0;
    locationsCreated = 0;

    jsonLog.open("./viewer/debug.json");
    debugLog.open("./kethbot/debug.log");
}

Logger::~Logger()
{
    jsonLog.close();
    debugLog.close();
}

void Logger::saveJson()
{
    debugLog << "Saving JSON... ";
    std::string outputData = debugOutput.write(debugNode);
    jsonLog.setText(outputData);
    debugLog << "[DONE]" << endl;
}


void Logger::logError(std::string error)
{
    debugLog << "[ERROR] [TURN #" << (int)state.turn << "]: " << error << endl;
    errors++;

}

void Logger::logWarning(std::string warning)
{
    debugLog << "[ERROR WARNING] [TURN #" << (int)state.turn << "]: " << warning << endl;
    warnings++;
}

void Logger::logPreState(bool init)
{
    char key[15];
    std::stringstream os;
    sprintf(key, "%03i", state.turn);

    if (init) {
        logger.debugLog << "INPUT DATA:" << endl;
        logger.debugLog << "cols: " << (int)state.cols << endl;
        logger.debugLog << "rows: " << (int)state.rows << endl;
        logger.debugLog << "players: " << (int)state.players << endl;
        logger.debugLog << "attackRadius: " << (double)state.attackradius << endl;
        logger.debugLog << "spawnRadius: " << (double)state.spawnradius << endl;
        logger.debugLog << "viewRadius: " << (double)state.viewradius << endl << endl;
    } else {
        debugNode[key]["ants_alive"] = state.ants.size();

        os << state;
        debugNode[key]["visible_map"] = os.str();
        os.str("");

        os << (float)state.timer.getTime() << "ms";
        debugNode[key]["time_taken"] = os.str();
        os.str("");

        os << (int)state.approxPlayers;
        debugNode[key]["approx_players"] = os.str();
        os.str("");

        if (state.structuralAnts.size() > 0) {

            std::list<Ant*>::iterator iter_ant;
            for (iter_ant = state.structuralAnts.begin(); iter_ant != state.structuralAnts.end(); iter_ant++)
            {
                Ant* ant = (*iter_ant);
                if (ant) {
                    char key2[32];
                    sprintf(key2, "#%ix%i", ant->getLocation().row, ant->getLocation().col);
                    debugNode[key][key2] = ant->id;
                } else {
                    logger.logError("Found a removed structural ant");
                }

            }
        }
        saveJson();
    }
}

void Logger::logMapState()
{
    char key[15];
    sprintf(key, "%03i", state.turn);

    std::stringstream os;

    os << state;
    debugNode[key]["visible_map"] = os.str();
    os.str("");

    os << (double)state.visibilityCoverage;
    debugNode[key]["visibility_coverage"] = os.str();
    os.str("");

    os << (double)state.areaCoverage;
    debugNode[key]["area_coverage"] = os.str();
    os.str("");

    os << (double)bot.areaCoverageGoal();
    debugNode[key]["area_coverage_goal"] = os.str();
    os.str("");

    os << (double)bot.getExpandForce();
    debugNode[key]["expand_force"] = os.str();
    os.str("");

    saveJson();
}


void Logger::logPostState()
{
    char key[15];
    std::stringstream os;

    sprintf(key, "%03i", state.turn);

    debugNode[key]["ants_structural"] = state.structuralAnts.size();

    if (state.structuralAnts.size() > 0) {

        std::list<Ant*>::iterator iter_ant;
        for (iter_ant = state.structuralAnts.begin(); iter_ant != state.structuralAnts.end(); iter_ant++)
        {
            Ant* ant = (*iter_ant);
            if (ant) {
                char key2[32];
                sprintf(key2, "#ant%i_x", ant->id);
                debugNode[key][key2] = (int)round(ant->physicalPosition.x);

                sprintf(key2, "#ant%i_y", ant->id);
                debugNode[key][key2] = (int)round(ant->physicalPosition.y);
            } else {
                logger.logError("Found a removed structural ant");
            }

        }
    }

    saveJson();
}
