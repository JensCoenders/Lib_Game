@echo off

echo Updating version... 
powershell.exe -File "..\resources\scripts\update_version.ps1" -ResourceFile ..\resources\resources.rc -VersionFile ..\src\game\types.h

<NUL set /p= Compiling resources file... 
windres ..\resources\resources.rc -o src\resources.o
echo Done

echo.