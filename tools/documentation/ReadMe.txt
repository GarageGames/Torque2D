Generates TorqueScript reference documentation.

***********
* Windows *
***********
Execute the "generateDocs.bat"

This should produce an output file in the sub-folder "input" followed by executing the Win32 Doxygen to generate the documentation in the output sub-folder.  The default configuration only outputs HTML however if you change the "scriptReference.Windows.cfg" and change the line "GENERATE_HTMLHELP" to "= YES" you will also get a compiled CHM file.

********
* OS X *
********

Execute the "generateDocs.command"

This should produce an output file in the sub-folder "input" followed by executing the Win32 Doxygen to generate the documentation in the output sub-folder.  The default configuration only outputs HTML.

The latest Doxygen for OSX can be found here: http://www.stack.nl/~dimitri/doxygen/download.html  Shortly the OSX version will be placed into the repository alongside the DMG version.
