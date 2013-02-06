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

#ifndef _CONSOLE_NAMESPACE_H
#define _CONSOLE_NAMESPACE_H

#ifndef _STRINGTABLE_H_
#include "string/stringTable.h"
#endif
#ifndef _VECTOR_H_
#include "collection/vector.h"
#endif
#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

//-----------------------------------------------------------------------------

class ExprEvalState;
class CodeBlock;
class AbstractClassRep;

//-----------------------------------------------------------------------------

class Namespace
{
    enum {
        MaxActivePackages = 512,
    };

    static U32 mNumActivePackages;
    static U32 mOldNumActivePackages;
    static StringTableEntry mActivePackages[MaxActivePackages];
public:
    StringTableEntry mName;
    StringTableEntry mPackage;

    Namespace *mParent;
    Namespace *mNext;
    AbstractClassRep *mClassRep;
    U32 mRefCountToParent;
    const char* mUsage;
    // Script defined usage strings need to be cleaned up. This
    // field indicates whether or not the usage was set from script.
    bool mCleanUpUsage;

    struct Entry
    {
        enum {
            GroupMarker                  = -3,
            OverloadMarker               = -2,
            InvalidFunctionType          = -1,
            ScriptFunctionType,
            StringCallbackType,
            IntCallbackType,
            FloatCallbackType,
            VoidCallbackType,
            BoolCallbackType
        };

        Namespace *mNamespace;
        Entry *mNext;
        StringTableEntry mFunctionName;
        S32 mType;
        S32 mMinArgs;
        S32 mMaxArgs;
        const char *mUsage;
        StringTableEntry mPackage;

        CodeBlock *mCode;
        U32 mFunctionOffset;
        union CallbackUnion {
            StringCallback mStringCallbackFunc;
            IntCallback mIntCallbackFunc;
            VoidCallback mVoidCallbackFunc;
            FloatCallback mFloatCallbackFunc;
            BoolCallback mBoolCallbackFunc;
            const char* mGroupName;
        } cb;
        Entry();
        void clear();

        const char *execute(S32 argc, const char **argv, ExprEvalState *state);

    };
    Entry *mEntryList;

    Entry **mHashTable;
    U32 mHashSize;
    U32 mHashSequence;  ///< @note The hash sequence is used by the autodoc console facility
                        ///        as a means of testing reference state.

    Namespace();
    ~Namespace();
    void addFunction(StringTableEntry name, CodeBlock *cb, U32 functionOffset, const char* usage = NULL);
    void addCommand(StringTableEntry name,StringCallback, const char *usage, S32 minArgs, S32 maxArgs);
    void addCommand(StringTableEntry name,IntCallback, const char *usage, S32 minArgs, S32 maxArgs);
    void addCommand(StringTableEntry name,FloatCallback, const char *usage, S32 minArgs, S32 maxArgs);
    void addCommand(StringTableEntry name,VoidCallback, const char *usage, S32 minArgs, S32 maxArgs);
    void addCommand(StringTableEntry name,BoolCallback, const char *usage, S32 minArgs, S32 maxArgs);

    void addOverload(const char *name, const char* altUsage);

    void markGroup(const char* name, const char* usage);
    char * lastUsage;

    void getEntryList(Vector<Entry *> *);

    Entry *lookup(StringTableEntry name);
    Entry *lookupRecursive(StringTableEntry name);
    Entry *createLocalEntry(StringTableEntry name);
    void buildHashTable();
    void clearEntries();
    bool classLinkTo(Namespace *parent);
    bool unlinkClass(Namespace *parent);

    const char *tabComplete(const char *prevText, S32 baseLen, bool fForward);

    static U32 mCacheSequence;
    static DataChunker mCacheAllocator;
    static DataChunker mAllocator;
    static void trashCache();
    static Namespace *mNamespaceList;
    static Namespace *mGlobalNamespace;

    static void init();
    static void shutdown();
    static Namespace *global();

    static Namespace *find(StringTableEntry name, StringTableEntry package=NULL);

    static bool canTabComplete(const char *prevText, const char *bestMatch, const char *newText, S32 baseLen, bool fForward);

    static void activatePackage(StringTableEntry name);
    static void deactivatePackage(StringTableEntry name);
    static void dumpClasses( bool dumpScript = true, bool dumpEngine = true );
    static void dumpFunctions( bool dumpScript = true, bool dumpEngine = true );
    static void printNamespaceEntries(Namespace * g, bool dumpScript = true, bool dumpEngine = true);
    static void unlinkPackages();
    static void relinkPackages();
    static bool isPackage(StringTableEntry name);
};

#endif // _CONSOLE_NAMESPACE_H
