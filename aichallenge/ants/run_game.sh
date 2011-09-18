#!/usr/bin/env sh -x
./playgame.py --engine_seed 42 --player_seed 42 --food none --end_wait=0.25 --verbose --log_dir game_logs --turns 30 --turns 100 --map_file maps/random4.txt "python dist/sample_bots/python/HunterBot.py" "python dist/sample_bots/python/keth.py" "python bots/bro/bro.py" "python dist/sample_bots/python/HunterBot.py"

