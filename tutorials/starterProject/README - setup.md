## What Is This `StarterProject` Directory

`starterProject` is a template project for starting a Torque2D game from scratch.  You don't have to start here, but it should be easy to do so.

## What Does a Torque2D Game Project Consist Of?

In its simplest form, a project consists of a top-level directory with the following contents:

* the Torque2D game engine
* a `main.cs` script file, which the engine runs when it starts
* almost certainly a `modules` directory containing one or more modules of game assets and scripts
* almost certainly a bootstrap module, such as `AppCore`, to setup common variables and other required bits for the engine.

## How To Start With This `starterProject`

1. If you do not have a copy of the Torque2D binary, obtain or compile one.
2. Copy this "startProject" directory to wherever you want to create the project.  This becomes the main project directory.
3. If you are running Windows, copy the following files from your Torque2D original to this directory:
  * Torque2D.exe
  * OpenAL32.dll
	* unicows.dll
4. If you are running OSX, copy the following files from your Torque2D original to this directory:
  * Torque2D(.app)
5. Go to your Torque2D original and copy the modules/AppCore directory to this projects modules/ directory.
