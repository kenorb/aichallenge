#!/usr/bin/env sh -x
if [ -n "$1" ]; then
  TURNS=$1
else
  TURNS=100
fi
./playgame.py $2 $3 $4 $5 $6 -R -S -I -O -E --engine_seed 42 --player_seed 42 --food 1 --end_wait=0.25 -v --log_dir game_logs --turns $TURNS --map_file maps/random4.txt "wine bots/KethBot/KethBot1908.exe" "java -jar bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar" "python bots/bro/bro.py" "wine bots/KethBot/KethBot1809.exe"
# -e
# "python dist/sample_bots/python/HunterBot.py" "python dist/sample_bots/python/GreedyBot.py"
# attackradius2 is 5. viewradius2 is 77. spawnradius2 is 1.

