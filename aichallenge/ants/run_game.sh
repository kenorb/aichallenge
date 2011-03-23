#!/usr/bin/env sh -x
./playgame.py -v --seed 42 --output_dir viewer --turns 100 --map_file maps/random4.txt "python dist/sample_bots/python/HunterBot.py" "python dist/sample_bots/python/keth.py" "python dist/sample_bots/python/bro.py" "python dist/sample_bots/python/HunterBot.py" > /dev/null

