@echo off
if "%eclipse%"=="true" (
	set rel_path=.
) else (
	set rel_path=..\..\
)

echo Cleaning 'Release' directory...
if exist "%rel_path%\Release\src" (rmdir /Q /S "%rel_path%\Release\src")
if exist "%rel_path%\Release\*.zip" (del /F "%rel_path%\Release\*.zip")
	
<NUL set /p= Enter version: 
set /p version=

<NUL set /p= Packing... 
powershell.exe -nologo -noprofile -command "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::CreateFromDirectory('%rel_path%\Release', '.\jens_game_%version%.zip'); }"
if %errorlevel%==0 (
	echo Done
)

<NUL set /p= Moving to 'Release' directory... 
move /Y ".\jens_game_%version%.zip" "%rel_path%\Release\jens_game_%version%.zip" >NUL
echo Done

if not "%eclipse%"=="true" (
	pause
)