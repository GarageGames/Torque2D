#!/bin/sh

cd "`dirname "$0"`"

# --- Clean up the junk in the root of the scriptDocs folder ---
rm scriptDocs/*.*

cd scriptDocs/project
sh generateProjects.command
cd buildFiles
sh compile.command
cd ../../..

# --- Dump the engine docs ---
scriptDocs/project/game/ScriptDoc.app/Contents/MacOS/ScriptDoc

# --- Build the doxygen docs ---
cd ../Engine/bin/doxygen
doxygen scriptReference.mac.cfg
cd ../../../Documentation

# --- Build docset ---

cd scriptDocs
make
rm -rf ../com.garagegames.TorqueScript.docset
mv -f com.garagegames.TorqueScript.docset ..
rm *.* Makefile
