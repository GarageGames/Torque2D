Torque2D
========

MIT Licensed Open Source version of Torque 2D from GarageGames.

You can find the main Wiki page [here](https://github.com/GarageGames/Torque2D/wiki)

This branch is dedicated to making stuff work in Emscripten. In order to compile you'll need the following:

* Emscripten
* Modified Emscripten libraries (to fix OpenAL and OpenGL)
* CMake
* Pre-generated font files
* Web server to run the resulting page

The required font files as well as emscripten changes are all supplied in a [downloadable archive](http://stuff.cuppadev.co.uk/torque2d_emscripten_files.zip)

## Setting up Emscripten

NOTE: these instructions assume you are using a Linux or OS X box. Other platforms may vary.

First you need to install emscripten itself. [The emscripten wiki](https://github.com/kripken/emscripten/wiki/Tutorial) is a good starting point.

You also may need to modify emscriptens `library_gl.js` and `library_openal.js` in order to add some missing functionality to OpenAL and remove a rather annoying OpenGL logger statement. Modified versions of both files as well as a diff are included in the downloadable archive.

Note that the build has only been tested from emscripten built from commit `1a007b1631509b9d72499a8f4402294017ee04dc` with the supplied changes, though you should be able to use later versions as well. The emscripten changes may or may not be required in a later emscripten build.

When you have the emscripten toolchain setup, you'll need to modify `engine/compilers/emscripten/generate.sh` and `engine/compilers/emscripten/generate_release.sh` in order to point it to the emscripten toolchain. For example if you have emscripten installed in `/Users/kork/emscripten`, the top should look like this:

	EMSCRIPTEN_PATH=/Users/kork/emscripten

Before building, make sure you have generated all the neccesary font files. The current list of fonts required is stored in the "FontAssetFiles" list in `emscripten/assets/CMakeLists.txt`. Failing that the downloadable archive includes a prebuilt set of required font files.

When you are ready to build, run the following from a terminal:

	cd engine/compilers/emscripten
	sh ./generate.sh
	make

Or if you want to make a release build:

	cd engine/compilers/emscripten
	sh ./generate_release.sh
	make

After a long while you should get several files in the emscripten folder, the most important ones being "Torque2D.html", "Torque2D.js" and "Torque2D.data". If you want to test things out, simply copy these files to a web server and open Torque2D.html. Alternatively you can use python to start a http server and check it out, e.g.:

	python -m SimpleHTTPServer

Then open http://(the address)/Torque2D.html

Have fun!

