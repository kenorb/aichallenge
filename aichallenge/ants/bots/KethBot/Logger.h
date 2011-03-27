#ifdef __DEBUG

#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

Json::StyledWriter debugOutput;
Json::Value debugNode;

void logState(State* state, bool init)
{
    if (init) {
        state->debugLog << "INPUT DATA:" << endl;
        state->debugLog << "cols: " << (int)state->cols << endl;
        state->debugLog << "rows: " << (int)state->rows << endl;
        state->debugLog << "players: " << (int)state->players << endl;
        state->debugLog << "attackRadius: " << (double)state->attackradius << endl;
        state->debugLog << "spawnRadius: " << (double)state->spawnradius << endl;
        state->debugLog << "viewRadius: " << (double)state->viewradius << endl << endl;
    } else {
        char key[11];
        std::stringstream os;

        sprintf(key, "%03i", state->turn);
        debugNode[key]["ants_alive"] = state->ants.size();

        os << (*state);
        debugNode[key]["map_visible"] = os.str();
        os.str("");

        os << (float)state->timer.getTime() << "ms";
        debugNode[key]["time_taken"] = os.str();
        os.str("");

        std::string outputData = debugOutput.write(debugNode);
        state->jsonLog.setText(outputData);
    }
}

#endif // LOGGER_H_INCLUDED

#endif
