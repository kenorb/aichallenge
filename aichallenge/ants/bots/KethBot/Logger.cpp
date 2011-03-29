#include "Logger.h"
#include <iostream>
#include <fstream>
#include <sstream>

Logger::Logger()
{
    jsonLog.open("./viewer/debug.json");
    debugLog.open("./kethbot/debug.log");
}

Logger::~Logger()
{
    jsonLog.close();
    debugLog.close();
}

void Logger::logPreState(State* state, bool init)
{
    if (init) {
        state->logger.debugLog << "INPUT DATA:" << endl;
        state->logger.debugLog << "cols: " << (int)state->cols << endl;
        state->logger.debugLog << "rows: " << (int)state->rows << endl;
        state->logger.debugLog << "players: " << (int)state->players << endl;
        state->logger.debugLog << "attackRadius: " << (double)state->attackradius << endl;
        state->logger.debugLog << "spawnRadius: " << (double)state->spawnradius << endl;
        state->logger.debugLog << "viewRadius: " << (double)state->viewradius << endl << endl;
    } else {
        char key[11];
        std::stringstream os;

        sprintf(key, "%03i", state->turn);
        debugNode[key]["ants_alive"] = state->ants.size();

        os << (*state);
        debugNode[key]["visible_map"] = os.str();
        os.str("");

        os << (float)state->timer.getTime() << "ms";
        debugNode[key]["time_taken"] = os.str();
        os.str("");

        if (state->structuralAnts.size() > 0) {

            std::list<Ant*>::iterator iter_ant;
            for (iter_ant = state->structuralAnts.begin(); iter_ant != state->structuralAnts.end(); iter_ant++)
            {
                Ant* ant = (*iter_ant);
                if (ant) {
                    char loc[32];
                    sprintf(loc, "#%ix%i", ant->getLocation().row, ant->getLocation().col);
                    debugNode[key][loc] = ant->id;
                } else {
                    state->logError("Found a removed structural ant");
                }

            }
        }

        std::string outputData = debugOutput.write(debugNode);
        state->logger.jsonLog.setText(outputData);
    }
}

void Logger::logMapState(State* state)
{
    char key[11];
    sprintf(key, "%03i", state->turn);

    std::stringstream os;

    os << (*state);
    debugNode[key]["visible_map"] = os.str();
    os.str("");

    os << (double)state->visibilityCoverage;
    debugNode[key]["visibility_coverage"] = os.str();
    os.str("");

    std::string outputData = debugOutput.write(debugNode);
    state->logger.jsonLog.setText(outputData);
}


void Logger::logPostState(State* state)
{
    char key[11];
    std::stringstream os;

    sprintf(key, "%03i", state->turn);

    debugNode[key]["ants_structural"] = state->structuralAnts.size();

    std::string outputData = debugOutput.write(debugNode);
    state->logger.jsonLog.setText(outputData);
}

