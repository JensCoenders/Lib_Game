@echo off
if "%eclipse%"=="true" (
	set rel_path=.
) else (
	set rel_path=..\..\
)

echo Cleaning directories...
if exist "%rel_path%\build.zip" (del /Q /F "%rel_path%\build.zip")
if exist "%rel_path%\Release\src" (rmdir /S /Q "%rel_path%\Release\src")

<NUL set /p= Packing... 
powershell.exe -nologo -noprofile -command "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::CreateFromDirectory('%rel_path%\Release', '%rel_path%\build.zip'); }"
echo Done

if not "%eclipse%"=="true" (
	pause
)