import os
import time

for seed in range(0, 1000):
    fh = os.popen("PostCompile.bat STATS_ONLY " + str(seed))
    output = fh.read()
    winplayer = ""
    if output.find("bot 0 eliminated") >= 0:
        winplayer = "_W"

    if output.find("bot 1 eliminated") >= 0:
        winplayer = "_L"

    fname = ('games/seed') + str(seed) + str(winplayer) + ('.txt')
    f = open(fname, 'w')
    print 'Saved game to ' + fname
    f.write(output)
    fh.close()

