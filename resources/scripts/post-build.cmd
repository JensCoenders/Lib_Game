@echo off

echo.

echo Copying DLL files...
xcopy /Y /Q "..\resources\dll" ".\"

echo Copying asset files...
if exist ".\assets" (
	rmdir /Q /S ".\assets"
)

mkdir .\assets
xcopy /Y /Q /S "..\resources\assets" ".\assets"