//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

ConsoleFunctionGroupBegin( Packages, "Functions relating to the control of packages.");

/*! @defgroup PackageFunctions Packages
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Use the isPackage function to check if the name or ID specified in packageName is a valid package.
    @param packagename The name or ID of an existing package.
    @return Returns true if packageName is a valid package, false otherwise.
    @sa activatePackage, deactivatePackage
*/
ConsoleFunctionWithDocs(isPackage, ConsoleBool,2,2, ( packageName ))
{
   StringTableEntry packageName = StringTable->insert(argv[1]);
   return Namespace::isPackage(packageName);
}

/*! Use the activatePackage function to activate a package definition and to re-define all functions named within this package with the definitions provided in the package body.
    This pushes the newly activated package onto the top of the package stack.
    @param packagename The name or ID of an existing package.
    @return No return value.
    @sa deactivatePackage, isPackage
*/
ConsoleFunctionWithDocs(activatePackage, ConsoleVoid,2,2, ( packageName ))
{
   StringTableEntry packageName = StringTable->insert(argv[1]);
   Namespace::activatePackage(packageName);
}

/*! Use the deactivatePackage function to deactivate a package definition and to pop any definitions from this package off the package stack.
    This also causes any subsequently stacked packages to be popped. i.e. If any packages were activated after the one specified in packageName, they too will be deactivated and popped.
    @param packagename The name or ID of an existing package.
    @return No return value.
    @sa activatePackage, isPackage
*/
ConsoleFunctionWithDocs(deactivatePackage, ConsoleVoid,2,2, ( packageName ))
{
   StringTableEntry packageName = StringTable->insert(argv[1]);
   Namespace::deactivatePackage(packageName);
}

ConsoleFunctionGroupEnd(Packages);

/*! @} */ // group PackageFunctions