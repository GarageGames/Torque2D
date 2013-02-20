@echo off
setlocal

REM --- Dump the engine docs ---
if not exist "..\..\Torque2D.exe" (
	echo.
	echo The Torque 2D executable was not found.
	echo.

	endlocal
	exit /b 1
)
if exist "input\docDump.txt" del /Q "input\docDump.txt"
"../../Torque2D.exe" generateDocs.cs
if not exist "input\docDump.txt" (
    echo.
    echo The Torque 2D documentation was not dumped.
    echo.

    endlocal
    exit /b 1
)

REM --- Build the doxygen docs ---
del /Q "output\html"
svn checkout "https://github.com/GarageGames/Torque2D.git/branches/gh-pages" "output\html"
doxygen.exe scriptReference.Windows.cfg

REM --- Add the newly generated docs to the gh-pages branch
cd "output\html"
svn add *.*
svn commit -m "- Updated reference"

REM --- Build the CHM ---
REM hhc.exe "output\html\index.hhp"
endlocal
