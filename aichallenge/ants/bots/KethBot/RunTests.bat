@ECHO OFF
:LOOP

START /MIN /W PostCompile.bat DEBUG
SET ERRORMSG=
FOR /F "tokens=*" %%f IN ('FINDSTR /C:"ERROR" "c:\[work]\Google AI\Ants\aichallenge\ants\kethbot\debug.log"') DO SET ERRORMSG=%%f
FOR /F "tokens=1-5 delims=:" %%d IN ("%time%") DO SET NEWFILE=debug_%%e-%%f.log
RENAME "c:\[work]\Google AI\Ants\aichallenge\ants\kethbot\debug.log" %NEWFILE%
FINDSTR /C:"ERROR" "c:\[work]\Google AI\Ants\aichallenge\ants\kethbot\%NEWFILE%" > nul
IF NOT ErrorLevel 1 (
ECHO "IN FILE %NEWFILE% FOUND %ERRORMSG%" >> "c:\[work]\Google AI\Ants\aichallenge\ants\kethbot\error.log"
@ECHO ON
@ECHO.
@ECHO/|set /p =%ERRORMSG%
@ECHO OFF
)
IF "%ERRORMSG%" == "" (
@echo/|set /p =.
)
FINDSTR /C:"GAME END" "c:\[work]\Google AI\Ants\aichallenge\ants\kethbot\%NEWFILE%" > nul
IF ErrorLevel 1 (
ECHO "IN FILE %NEWFILE% GAME CRASHED?" >> "c:\[work]\Google AI\Ants\aichallenge\ants\kethbot\error.log"
@ECHO ON
@ECHO.
@ECHO GAME CRASHED? %NEWFILE%
@ECHO OFF
)
GOTO LOOP