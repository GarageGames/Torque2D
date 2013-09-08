Generates Torque2D and TorqueScript reference documentation.

Use the manual (A) or automated (B) method.  Note that the automated method needs some updating and testing

***************

A) Manual

This should work for PC or Mac (or anywhere doxygen and git/svn run)

	1. start in this directory
	2. run  doxygen config/torque2DReference.cfg (will produce ./output/torque2DDocs and torque2D.tag)
	3. run  doxygen config/torqueScriptReference.cfg (will produce ./output/torqueScriptDocs)
	4. "push" output/torque2DDocs and output/torqueScriptDocs to gh-pages

***************

B) Automatic

!! the following is an automated method that needs to be updated !!

* Windows ***
Execute the "generateDocs.bat"
This should produce an output file in the sub-folder "input" followed by executing the Win32 Doxygen to generate the documentation in the output sub-folder.  The default configuration only outputs HTML however if you change the "scriptReference.Windows.cfg" and change the line "GENERATE_HTMLHELP" to "= YES" you will also get a compiled CHM file.

* OS X ***

Execute the "generateDocs.command"
This should produce an output file in the sub-folder "input" followed by executing the Win32 Doxygen to generate the documentation in the output sub-folder.  The default configuration only outputs HTML.
The latest Doxygen for OSX can be found here: http://www.stack.nl/~dimitri/doxygen/download.html  Shortly the OSX version will be placed into the repository alongside the DMG version.
