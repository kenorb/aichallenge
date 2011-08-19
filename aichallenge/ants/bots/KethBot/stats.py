import os
import time

fh = os.popen("play_one_game.cmd")
output = fh.read()
    
fh.close()
print output

