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

#ifndef _SCENE_OBJECT_SET_H_
#define _SCENE_OBJECT_SET_H_

#ifndef _SCENE_OBJECT_LIST_H_
#include "2d/sceneobject/SceneObjectList.h"
#endif

#ifndef _FIND_ITERATOR_H_
#include "collection/findIterator.h"
#endif

#ifndef _SIMDICTIONARY_H_
#include "sim/simDictionary.h"
#endif

#ifndef _SIM_OBJECT_H_
#include "sim/simObject.h"
#endif

#ifndef _TAML_CHILDREN_H_
#include "persistence/taml/tamlChildren.h"
#endif

//-----------------------------------------------------------------------------

class SceneObject;

//-----------------------------------------------------------------------------

class SceneObjectSet : public SimObject, public TamlChildren
{
   typedef SimObject Parent;

protected:
    SceneObjectList mObjectList;

public:
    SceneObjectSet()
    {
        VECTOR_SET_ASSOCIATION(mObjectList);
    }

    ~SceneObjectSet()
    {
    }

    typedef SceneObjectList::iterator iterator;
    typedef SceneObjectList::value_type value;

    inline SceneObject* front( void )    { return mObjectList.front(); }
    inline SceneObject* first( void )    { return mObjectList.first(); }
    inline SceneObject* last( void )		{ return mObjectList.last(); }
    inline bool empty( void )			{ return mObjectList.empty();   }
    inline S32 size( void ) const		{ return mObjectList.size(); }
    inline iterator begin( void )		{ return mObjectList.begin(); }
    inline iterator end( void )			{ return mObjectList.end(); }
    value operator[] (S32 index)         { return mObjectList[U32(index)]; }

    inline iterator find( iterator first, iterator last, SceneObject *obj ) { return ::find(first, last, obj); }
    inline iterator find( SceneObject *obj ) { return ::find(begin(), end(), obj); }

    template <typename T> inline bool containsType( void )
    {
        for( iterator itr = begin(); itr != end(); ++itr )
        {
            if ( dynamic_cast<T*>(*itr) != NULL )
                return true;
        }

        return false;
    }

    virtual bool reOrder( SceneObject *obj, SceneObject *target=0 );
    SceneObject* at(S32 index) const { return mObjectList.at(index); }

    void deleteObjects( void );
    void clear();

    virtual void onRemove();
    virtual void onDeleteNotify(SimObject *object);

    virtual void addObject(SceneObject*);
    virtual void removeObject(SceneObject*);

    virtual void pushObject(SceneObject*);
    virtual void popObject();

    void bringObjectToFront(SceneObject* obj) { reOrder(obj, front()); }
    void pushObjectToBack(SceneObject* obj) { reOrder(obj, NULL); }

    virtual U32 getTamlChildCount( void ) const { return (U32)size(); }

    virtual SimObject* getTamlChild( const U32 childIndex ) const
    {
        // Sanity!
        AssertFatal( childIndex < (U32)size(), "SceneObjectSet::getTamlChild() - Child index is out of range." );

        // For when the assert is not used.
        if ( childIndex >= (U32)size() )
            return NULL;

        return (SimObject*)at( childIndex );
    }

    virtual void addTamlChild( SimObject* pSimObject );

    void callOnChildren( const char * method, S32 argc, const char *argv[], bool executeOnChildGroups = true );

    virtual SimObject *findObject(const char *name);
    SceneObject*	findObjectByInternalName(const char* internalName, bool searchChildren = false);

    DECLARE_CONOBJECT(SceneObjectSet);

#ifdef TORQUE_DEBUG
    inline void _setVectorAssoc( const char *file, const U32 line ) { mObjectList.setFileAssociation( file, line ); }
#endif
};

#ifdef TORQUE_DEBUG
#  define SCENEOBJECT_SET_ASSOCIATION( x ) x._setVectorAssoc( __FILE__, __LINE__ )
#else
#  define SCENEOBJECT_SET_ASSOCIATION( x )
#endif

//-----------------------------------------------------------------------------

class SceneObjectSetIterator
{
protected:
    struct Entry
    {
        SceneObjectSet* set;
        SceneObjectSet::iterator itr;
    };

    class Stack : public Vector<Entry> {
    public:
        void push_back(SceneObjectSet*);
    };
    Stack stack;

public:
    SceneObjectSetIterator(SceneObjectSet*);
    SceneObject* operator++();
    SceneObject* operator*() {
        return stack.empty()? 0: *stack.last().itr;
    }
};

#endif // _SCENE_OBJECT_SET_H_
