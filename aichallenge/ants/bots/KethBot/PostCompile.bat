@ECHO OFF
if NOT "%1" == "STATS_ONLY" @ECHO ON
cd "c:\[work]\Google AI\Ants\aichallenge\ants\"
if NOT "%2" == "" (
playgame.py --seed %2 --attack closest --output_dir viewer --turns 500 --map_file maps/test.txt "python dist\sample_bots\python\HunterBot.py" "c:\[work]\C++\KethBot\KethBot.exe"
)
if "%2" == "" (
playgame.py --attack closest --output_dir viewer --turns 100 --map_file maps/test.txt "python dist\sample_bots\python\HunterBot.py" "c:\[work]\C++\KethBot\KethBot.exe"
)


if "%1" == "SHOWLOG" GOTO END
if "%1" == "DEBUG" GOTO END
if "%1" == "STATS_ONLY" GOTO END
cd "C:\Users\KETH\AppData\Local\Google\Chrome\Application\"
start chrome.exe "http://127.0.0.1:8000/viewer/viewer.html"
:END
exit
