@ECHO ON
if NOT "%1" == "STATS_ONLY" @ECHO ON
cd "c:\[work]\Google AI\Ants\newest\ants\"

playgame.py --attack damage --seed 4 --verbose --output_dir "viewer" --turns 100 --map_file maps/compile.txt "java -jar stocha.jar" "c:\[work]\C++\KethBot\KethBot.exe" "java -jar stocha.jar" "java -jar stocha.jar" 


if "%1" == "SHOWLOG" GOTO END
if "%1" == "DEBUG" GOTO END
if "%1" == "STATS_ONLY" GOTO END
cd "C:\Users\KETH\AppData\Local\Google\Chrome\Application\"
start chrome.exe "http://127.0.0.1:8000/viewer/viewer.html"
:END
