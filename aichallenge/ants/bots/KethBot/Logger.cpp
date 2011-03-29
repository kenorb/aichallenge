#include "Logger.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "AI/Bot.h"

Logger::Logger()
{
    errors = 0;
    warnings = 0;

    jsonLog.open("./viewer/debug.json");
    debugLog.open("./kethbot/debug.log");
}

Logger::~Logger()
{
    jsonLog.close();
    debugLog.close();
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
    if (init) {
        logger.debugLog << "INPUT DATA:" << endl;
        logger.debugLog << "cols: " << (int)state.cols << endl;
        logger.debugLog << "rows: " << (int)state.rows << endl;
        logger.debugLog << "players: " << (int)state.players << endl;
        logger.debugLog << "attackRadius: " << (double)state.attackradius << endl;
        logger.debugLog << "spawnRadius: " << (double)state.spawnradius << endl;
        logger.debugLog << "viewRadius: " << (double)state.viewradius << endl << endl;
    } else {
        char key[11];
        std::stringstream os;

        sprintf(key, "%03i", state.turn);
        debugNode[key]["ants_alive"] = state.ants.size();

        os << state;
        debugNode[key]["visible_map"] = os.str();
        os.str("");

        os << (float)state.timer.getTime() << "ms";
        debugNode[key]["time_taken"] = os.str();
        os.str("");

        if (state.structuralAnts.size() > 0) {

            std::list<Ant*>::iterator iter_ant;
            for (iter_ant = state.structuralAnts.begin(); iter_ant != state.structuralAnts.end(); iter_ant++)
            {
                Ant* ant = (*iter_ant);
                if (ant) {
                    char loc[32];
                    sprintf(loc, "#%ix%i", ant->getLocation().row, ant->getLocation().col);
                    debugNode[key][loc] = ant->id;
                } else {
                    logger.logError("Found a removed structural ant");
                }

            }
        }

        std::string outputData = debugOutput.write(debugNode);
        logger.jsonLog.setText(outputData);
    }
}

void Logger::logMapState()
{
    char key[11];
    sprintf(key, "%03i", state.turn);

    std::stringstream os;

    os << state;
    debugNode[key]["visible_map"] = os.str();
    os.str("");

    os << (double)state.visibilityCoverage;
    debugNode[key]["visibility_coverage"] = os.str();
    os.str("");

    std::string outputData = debugOutput.write(debugNode);
    logger.jsonLog.setText(outputData);
}


void Logger::logPostState()
{
    char key[11];
    std::stringstream os;

    sprintf(key, "%03i", state.turn);

    debugNode[key]["ants_structural"] = state.structuralAnts.size();

    std::string outputData = debugOutput.write(debugNode);
    logger.jsonLog.setText(outputData);
}

