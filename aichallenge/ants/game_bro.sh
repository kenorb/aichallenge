#!/usr/bin/env sh -x
(netstat -na | grep .8000 > /dev/null) || sh ./python_server.sh > /dev/null &
./playgame.py --turns 200 -v --seed 42 --output_dir viewer/bro_viewer --map_file maps/random4.txt "wine bots/KethBot/KethBot.exe" "java -jar bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar" "python bots/bro/bro.py" "python dist/sample_bots/python/HunterBot.py" \
    && open http://localhost:8000/viewer/bro_viewer/viewer.html
