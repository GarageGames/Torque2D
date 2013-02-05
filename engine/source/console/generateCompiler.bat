@echo off
call bison.bat CMD CMDgram.c CMDgram.y . CMDgram.cc
..\..\bin\flex\flex -PCMD -oCMDscan.cc CMDscan.l
