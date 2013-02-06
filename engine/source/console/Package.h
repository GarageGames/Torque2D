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

#ifndef _PACKAGE_H
#define _PACKAGE_H


#ifdef PUAP_NAMESPACE_CHANGE

#ifndef _STRINGTABLE_H_
#include "string/stringTable.h"
#endif
#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif
#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#include "tHashTable.h"
#include "Namespace.h"


class ExprEvalState;
struct FunctionDecl;
class CodeBlock;
class AbstractClassRep;


class Package
{
    enum {
        MaxActivePackages = 512,
    };

public:
    StringTableEntry mName;

    Package *mParent;
    AbstractClassRep *mClassRep;
    U32 mRefCountToParent;
    const char* mUsage;
    // Script defined usage strings need to be cleaned up. This
    // field indicates whether or not the usage was set from script.
    bool mCleanUpUsage;
	int mActivePosition;


    Package();
    ~Package();
	
	typedef tHashTable<StringTableEntry, Namespace*>::Iterator nameSpaceIterator;
	typedef tHashTable<StringTableEntry, Package*>::Iterator packageIterator;
	tHashTable<StringTableEntry, Namespace*> *mChildNamespaceList;
	tHashTable<StringTableEntry, Package*> *mChildPackageList;

	Namespace *mDefaultNamespace;

	//-Mat, maybe put these into ExprEvalState
	static Package *smRootPackage;
	static Package *smMainPackage;

	static void init();
	static void shutdown();

	static Package *getRootPackage() { return smRootPackage; }
	static Package *getMainNamespacePackage() { return smMainPackage; }
	//this namespace be be the child of smMainPackage
	static Namespace *getMainNamespace() { return smMainPackage->getDefaultNamespace(); }

	//this will find the most recently activated package
	static Package *getCurrentPackage();

	static Package *findPackage(StringTableEntry);
	static Package *findAndCreatePackage(StringTableEntry name);

    static void activatePackage(StringTableEntry name);
    static void deactivatePackage(StringTableEntry name);
    static void dumpNamespaces( bool dumpScript = true, bool dumpEngine = true );
    static void unlinkPackages();
    static void relinkPackages();

	//Namespace::Entry *lookupEntryInActivePackages( StringTableEntry, StringTableEntry );
	static Namespace *lookupNamespaceInActivePackages( StringTableEntry);
	
	Namespace *getDefaultNamespace() { return mDefaultNamespace; }
	//-Mat functions to help with new Namespace organization	
	Namespace *findNamespace(StringTableEntry name);
	Namespace *findAndCreateNamespace(StringTableEntry name);

	Namespace *addAndCreateNamespace( StringTableEntry );
	Namespace *addNamespace( Namespace* );
	Namespace *swapNamespace( Namespace* );
	
	Package *addPackage(Package*);

	void activate(int activePosition) { mActivePosition = activePosition; }
	void deActivate() { mActivePosition = -1; }
	bool isActive() { return mActivePosition != -1; }

private:
	//private so that we can make sure no Namespace has a NULL parent because of this
	//use SwapNamespace to "remove" a namespace (it will get moved to the MainPackage)
	Namespace *removeNamespace( Namespace* );
};


#else
//normal TGB


#endif //PUAP_NAMESPACE_CHANGE


#endif// _PACKAGE_H
