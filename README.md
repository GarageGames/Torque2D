![Torque Logo](http://static.garagegames.com/static/pg/logokits/Torque-Logo_H.png)
## Torque 2D 3.0

MIT Licensed Open Source version of Torque 2D from GarageGames. Maintained by the T2D Steering Committee and contributions from the community.

Dedicated to 2D game development, Torque 2D is an extremely powerful, flexible, and fast C++ engine which has been used in hundreds of professional games. It is a true cross platform solution providing you access to Windows, OSX, Linux, iOS, Android, and the web - all from one codebase. It includes an OpenGL batched rendering system, Box2D physics, OpenAL audio, skeletal and spritesheet animation, automated asset management, a modular project structure, TAML object persistence, and a C-like scripting language.

### Branches

Here is an overview of the branches found in the Torque 2D repository: 

* **master:** this branch contains the current stable release code that can be used in a production environment. 
* **development:** this branch is dedicated to active development. It contains the latest bug fixes, new features, and other updates. All pull requests need to go to the development branch. While we try our best to test all incoming changes, it is possible for mistakes to slip in therefore this branch should always be considered unstable.
* **gh-pages:** this branch currently contains the html pages generated from doxygen for the engine and TorqueScript references.

### Precompiled Version

If you do not wish to compile the source code yourself, precompiled binary files for Windows and OSX are available from the [Torque 2D Release Page](https://github.com/GarageGames/Torque2D/releases).

### Building the Source

After downloading a copy of the source code, the following project files for each platform are provided for you and can be found in the `engine/compilers` folder.

* **Windows:** Visual Studio 2010, 2012, or 2013 (works with the free, "Express for Windows Desktop" version)
* **OSX:** Xcode
* **Linux:** Make
* **iOS:** Xcode_iOS
* **Android:** Eclipse
* **Web:** Emscripten/Cmake

See the [wiki](https://github.com/GarageGames/Torque2D/wiki) for available guides on platform setup and development.

### Batteries Included

![truck](http://t2dtutorials.com/img/Truck.png)

Running Torque 2D for the first time out of the box will start you off in the Sandbox. The Sandbox is a collection of over 30 simple "toys" (or modules) which demonstrate various features in T2D. The default toy is a side scrolling level with a monster truck. To see a list of the available modules/toys to choose from, click on the `Show Tools` button in the lower right corner of the screen.

Naturally all of the script code and assets for each toy are available to you in the modules folder to use as practical examples while learning T2D.

The Sandbox is also an excellent framework for rapidly prototyping your own game ideas - it allows for easy integration of additional modules and provides numerous debugging features, like console access and real-time metrics.

### Documentation

All documentation for the open source version of Torque 2D can be found on our [Github wiki page](https://github.com/GarageGames/Torque2D/wiki). It contains many tutorials, detailed technical information on engine systems, a script reference guide automatically generated from the source code, and articles on how to contribute to our open source development.

### Community

Don't go it alone! Join the active community around Torque 2D at GarageGames.com. Ask questions, talk about T2D and general game development topics, learn the latest news, or post a blog promoting your game or showing off additional engine features in your T2D fork.

* [Torque 2D Beginner Forum](http://www.garagegames.com/community/forums/84)
* [Torque 2D Professional Forum](http://www.garagegames.com/community/forums/85)
* [GarageGames Community Blogs](http://www.garagegames.com/community/blogs)
