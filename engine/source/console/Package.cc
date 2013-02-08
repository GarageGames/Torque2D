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

#ifdef PUAP_NAMESPACE_CHANGE

#include "platform/platform.h"
#include "console/console.h"

#include "console/ast.h"
#include "core/tAlgorithm.h"
#include "core/resManager.h"

#include "core/findMatch.h"
#include "console/consoleInternal.h"
#include "core/fileStream.h"
#include "console/compiler.h"


#include "Package.h"
#include "Namespace.h"
#include "tHashTable.h"

//-Mat contains ALL packages
Package *Package::smRootPackage = NULL;
//-Mat the main package for all non-packaged namespaces
Package *Package::smMainPackage = NULL;


Vector<Package *> gActivePackages;
Vector<Package *> gOldActivePackages;


Package::Package() {
	mName = NULL;
	mActivePosition = -1;
	mChildNamespaceList = new tHashTable< StringTableEntry, Namespace* >;
	mChildPackageList = new tHashTable< StringTableEntry, Package* >;
	Namespace *ret = findAndCreateNamespace( DEFAULT_PACKAGE_NAMESPACE_NAME );//make our default namespace
	mDefaultNamespace = ret;
	mParent = NULL;
}
Package::~Package() {
}


void Package::init()
{
   // create the root package
   smRootPackage = new Package;
   //constructInPlace( smRootPackage );
   smRootPackage->mName = StringTable->insert( "RootPackage" );


	//create main package
   smMainPackage = new Package;
   //constructInPlace( smMainPackage );
   smMainPackage->mName = StringTable->insert( DEFAULT_PACKAGE_NAME );

   //add to Main Package Root Package
   smRootPackage->addPackage( smMainPackage );

	//the main namespace will be the default namespace for the main Pacakge
   //this should always be the last active namespace
   activatePackage( smMainPackage->mName );
}

void Package::shutdown()
{
}

Package *Package::findPackage(StringTableEntry name) {

	if( name == NULL ) {
		//we cannot find this package so use the current one
		return Package::getCurrentPackage();
	}
	
	Package *currentPackage = smRootPackage->mChildPackageList->find( name ).getValue();

	return currentPackage;
}

Namespace *Package::findNamespace(StringTableEntry name) {
	
	Namespace *currentNamespace = mChildNamespaceList->find( name ).getValue();

	return currentNamespace;
}

Namespace *Package::findAndCreateNamespace(StringTableEntry name) {
	 Namespace *ret = findNamespace( name );
	 if( ret == NULL ) {
		ret = addAndCreateNamespace( name );
	 }
	 return ret;
}

Package *Package::addPackage( Package *ns ) {
	Package *existing = findPackage( ns->mName );
	if( existing != NULL ) {
		Con::warnf( "Package %s has already been added to the Root Package List", ns->mName );
		return ns;
	}
	smRootPackage->mChildPackageList->insertUnique( ns->mName, ns );
	ns->mParent = smRootPackage;
	return ns;
}


Namespace *Package::addAndCreateNamespace( StringTableEntry name ) {
	
	Namespace *ret = NULL;
	if( name != NULL ) {
		ret = findNamespace( name );
	}
	if( ret == NULL ) {
		ret = new Namespace( this, StringTable->insert( name ) );//add namespace to us
	}

	return ret;
}

Namespace *Package::addNamespace( Namespace *ns ) {
	if( ns->mOwner ) {
		if( ns->mOwner == this ) {
			return ns;//we alread own this Namespace
		}
		return swapNamespace( ns );//if it has a parent then swap with that parent
	}

	mChildNamespaceList->insertUnique( ns->mName, ns );
	ns->mOwner = this;
	return ns;
}

//Private, no one else needs to know about this,
//otherwise, it may be use innapropriately (Namespaces with NULL mparents)
Namespace *Package::removeNamespace( Namespace *ns ) {
	//check if it's in the list first
	mChildNamespaceList->erase( ns->mName );
	ns->mOwner = NULL;
	return ns;
}

Namespace *Package::swapNamespace( Namespace *ns ) {
	//Namespaces must ALWAYS belong to some Package, removing them just puts them in smMainPackage
	//so if that is "this", then do nothing
	if( this == smMainPackage ) {
		return ns;
	}
	
	//check if it's in the list first
	if( ns->mOwner == this ) {
		return ns;
	}
	ns->mOwner->removeNamespace( ns );
	//add it to the global instead, since we're not deleting it
	smMainPackage->addNamespace( ns );
	return ns;
}

Package *Package::findAndCreatePackage(StringTableEntry packageName ) {
	//Iterate through all our package to find this, if not, make a new one
	Package *package = findPackage( packageName );
	if( package == NULL ) {
		package = new Package;
		//constructInPlace(package);
		package->mName = packageName;
		smRootPackage->addPackage( package );
	}

	return package;
}

Namespace *Package::lookupNamespaceInActivePackages( StringTableEntry nsName ) {
	nsName = StringTable->insert( nsName );
	Package *currentPackage = NULL;
	Namespace *currentNS = NULL;
	S32 size = gActivePackages.size();
	for( S32 i = size; i > 0 ; i-- ) {
		currentPackage = gActivePackages[i-1];//-1 because size counts from 1
		currentNS = currentPackage->mChildNamespaceList->find( nsName ).getValue();
		if( currentNS ) {
			return currentNS;
		}
	}
	return NULL;
}
/*
Namespace::Entry *Package::lookupEntryInActivePackages( StringTableEntry entryName, StringTableEntry nsName ) {
	entryName = StringTable->insert( entryName );
	Package *currentPackage = NULL;
	Namespace *packageNS = NULL;
	Namespace::Entry *ret = NULL;
//if we're here, we've already checked our active package so only check the ones below us
S32 size = mActivePosition-1;
for( S32 i = size; i >= 0 ; i-- ) {
currentPackage = gActivePackages[i];
		packageNS = currentPackage->findNamespace( nsName );
		if( packageNS ) {
			ret = packageNS->lookupNoRecurse( entryName );
		}			
		//-Mat make sure the Namespace still matches
			if( ret != NULL ) {
				return ret;
			}
	}
	return NULL;
}
*/

Package *Package::getCurrentPackage() {
	//first, check if there are any activate, if so, return the most recently ectivated one
	U32 size = gActivePackages.size();
	if( size > 0 ) {
		Package *ret = gActivePackages[size-1];
		return ret;
	}

	//if there is no current Package, use the main Package
	return smMainPackage;
}


/*
	-Mat here we'll just iterate through the global package's list of packages and check each package's
	mActive == true
*/

void Package::activatePackage(StringTableEntry name)
{
   if(!name)
      return;

   name = StringTable->insert( name );


	S32 size = gActivePackages.size();
	Package *targetPackage = smRootPackage->findPackage( name );
	if( targetPackage ) {
		//see if it's already active
		for( S32 i = size; i > 0 ; i-- ) {
			Package *currentPackage = gActivePackages[i-1];//-1 because size counts from 1
			if( currentPackage->mName == name ) {
				if( currentPackage->isActive() ) {
					Con::warnf( "Package %s already active", currentPackage->mName );
					//-Mat may want to remove this from the list and add it to the bottom
					return;
				}
			}
		}
		//activate it and add it to the list size is array size so set it first to save the -1
		targetPackage->activate(gActivePackages.size());
		gActivePackages.push_back( targetPackage );
		return;
	} else {
		Con::errorf( "Package %s does not exist", name );
	}
}

void Package::deactivatePackage(StringTableEntry name)
{
   if(!name)
      return;

   name = StringTable->insert( name );

   // see if this one's already active
	U32 packageNumber = -1;
	U32 size = gActivePackages.size();
	for( S32 i = 0; i < size; i++ ) {
	   if( gActivePackages[i]->mName == name ) {
			packageNumber = i;
		   gActivePackages[i]->deActivate();
		   gActivePackages.erase( i );
			break;
		}
	   }
	if( packageNumber >= 0 ) {
		size = gActivePackages.size();
		for( S32 i = packageNumber; i <size; i++ ) {
			gActivePackages[i]->mActivePosition--;//to account for the one we just deleted
   }
	} else {
   Con::errorf( "Package %s does not exist", name );
}
}


void Package::unlinkPackages()
{
#if 0
	//-Mat warning, not using these two properly can screw up packages, make sure to relink these
	Package *currentPackage = NULL;
	//Deactivate all active packages
	gOldActivePackages = gActivePackages;
	gActivePackages.clear();
	//this one is always active
	activatePackage( smMainPackage->mName );
#endif
}

void Package::relinkPackages()
{
#if 0
	gActivePackages = gOldActivePackages;
	gOldActivePackages.clear();
#endif
}

ConsoleFunctionGroupBegin( Packages, "Functions relating to the control of packages.");

ConsoleFunction(isPackage,bool,2,2,"( packageName ) Use the isPackage function to check if the name or ID specified in packageName is a valid package.\n"
																"@param packagename The name or ID of an existing package.\n"
																"@return Returns true if packageName is a valid package, false otherwise.\n"
																"@sa activatePackage, deactivatePackage")
{
   StringTableEntry packageName = StringTable->insert(argv[1]);
   //-Mat do a lookup of the package name on the global package
   if( Package::findPackage( packageName ) != NULL ) {
	   return true;//such a package exists
   }
   return false;
}

ConsoleFunction(activatePackage, void,2,2,"( packageName ) Use the activatePackage function to activate a package definition and to re-define all functions named within this package with the definitions provided in the package body.\n"
																"This pushes the newly activated package onto the top of the package stack.\n"
																"@param packagename The name or ID of an existing package.\n"
																"@return No return value.\n"
																"@sa deactivatePackage, isPackage")
{
   StringTableEntry packageName = StringTable->insert(argv[1]);
   Package::activatePackage(packageName);
}

ConsoleFunction(deactivatePackage, void,2,2,"( packageName ) Use the deactivatePackage function to deactivate a package definition and to pop any definitions from this package off the package stack.\n"
																"This also causes any subsequently stacked packages to be popped. i.e. If any packages were activated after the one specified in packageName, they too will be deactivated and popped.\n"
																"@param packagename The name or ID of an existing package.\n"
																"@return No return value.\n"
																"@sa activatePackage, isPackage")
{
   StringTableEntry packageName = StringTable->insert(argv[1]);
   Package::deactivatePackage(packageName);
}

ConsoleFunctionGroupEnd( Packages );


#else
//normal TGB


#endif //PUAP_NAMESPACE_CHANGE
