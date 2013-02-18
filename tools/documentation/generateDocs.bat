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
rd "output\html"
doxygen.exe scriptReference.Windows.cfg

REM --- Build the CHM ---
REM hhc.exe "output\html\index.hhp"
endlocal
