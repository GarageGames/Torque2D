Debug Visualizers for Torque 2D.

Type visualizers for C++ types are specified in .natvis files. A natvis file is simply an xml file (with .natvis extension) that contains visualization rules for one or more types. At the start of each debugging session, Visual Studio processes any natvis files it can find in the following locations:

    -         %VSINSTALLDIR%\Common7\Packages\Debugger\Visualizers (requires admin access)
    -         %USERPROFILE%\My Documents\Visual Studio 2012\Visualizers\
    -         VS extension folders
	

More information can be found here:

http://blogs.msdn.com/b/vcblog/archive/2012/07/12/10329460.aspx
http://code.msdn.microsoft.com/Writing-type-visualizers-2eae77a2
