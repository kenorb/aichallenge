#!/usr/bin/env sh -x
(netstat -na | grep .8000 > /dev/null) || sh ./python_server.sh > /dev/null &
./playgame.py -v --seed 42 --output_dir viewer/bro_viewer --turns 20 --map_file maps/random4.txt "python dist/sample_bots/python/HunterBot.py" "python dist/sample_bots/python/keth.py" "python bots/bro/bro.py" "python dist/sample_bots/python/HunterBot.py" \
    && open http://localhost:8000/viewer/bro_viewer/viewer.html
