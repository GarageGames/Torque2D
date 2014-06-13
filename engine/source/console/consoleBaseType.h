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

#ifndef _CONSOLE_BASE_TYPE_H_
#define _CONSOLE_BASE_TYPE_H_

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

class ConsoleBaseType
{
protected:
   /// This is used to generate unique IDs for each type.
   static S32 smConsoleTypeCount;

   /// We maintain a linked list of all console types; this is its head.
   static ConsoleBaseType *smListHead;

   /// Next item in the list of all console types.
   ConsoleBaseType *mListNext;

   /// Destructor is private to avoid people mucking up the list.
   ~ConsoleBaseType();

   S32      mTypeID;
   dsize_t  mTypeSize;
   const char *mTypeName;
   const char *mInspectorFieldType;

public:

   /// @name cbt_list List Interface
   ///
   /// Interface for accessing/traversing the list of types.

   /// Get the head of the list.
   static ConsoleBaseType *getListHead();

   /// Get the item that follows this item in the list.
   ConsoleBaseType *getListNext() const
   {
      return mListNext;
   }

   /// Called once to initialize the console type system.
   static void initialize();

   /// Call me to get a pointer to a type's info.
   static ConsoleBaseType *getType(const S32 typeID);

   /// @}

   /// The constructor is responsible for linking an element into the
   /// master list, registering the type ID, etc.
   ConsoleBaseType(const S32 size, S32 *idPtr, const char *aTypeName);

   const S32 getTypeID() const { return mTypeID; }
   const dsize_t getTypeSize() const { return mTypeSize; }
   const char *getTypeName() const { return mTypeName; }

   void setInspectorFieldType(const char *type) { mInspectorFieldType = type; }
   const char *getInspectorFieldType() { return mInspectorFieldType; }

   virtual void setData(void *dptr, S32 argc, const char **argv, EnumTable *tbl, BitSet32 flag)=0;
   virtual const char *getData(void *dptr, EnumTable *tbl, BitSet32 flag )=0;
   virtual const char *getTypeClassName()=0;
   virtual const bool isDatablock() { return false; };
   virtual const char *prepData(const char *data, char *buffer, U32 bufferLen) { return data; };
   virtual StringTableEntry getTypePrefix( void ) const { return StringTable->EmptyString; }
};

#define DefineConsoleType( type ) extern S32 type;

#define ConsoleType( typeName, type, size, typePrefix ) \
   class ConsoleType##type : public ConsoleBaseType \
   { \
   public: \
      ConsoleType##type (const S32 aSize, S32 *idPtr, const char *aTypeName) : ConsoleBaseType(aSize, idPtr, aTypeName) { } \
      virtual void setData(void *dptr, S32 argc, const char **argv, EnumTable *tbl, BitSet32 flag); \
      virtual const char *getData(void *dptr, EnumTable *tbl, BitSet32 flag ); \
      virtual const char *getTypeClassName() { return #typeName ; } \
      virtual StringTableEntry getTypePrefix( void ) const { return StringTable->insert( typePrefix ); }\
   }; \
   S32 type = -1; \
   ConsoleType##type gConsoleType##type##Instance(size,&type,#type); \

#define ConsolePrepType( typeName, type, size, typePrefix ) \
   class ConsoleType##type : public ConsoleBaseType \
   { \
   public: \
      ConsoleType##type (const S32 aSize, S32 *idPtr, const char *aTypeName) : ConsoleBaseType(aSize, idPtr, aTypeName) { } \
      virtual void setData(void *dptr, S32 argc, const char **argv, EnumTable *tbl, BitSet32 flag); \
      virtual const char *getData(void *dptr, EnumTable *tbl, BitSet32 flag ); \
      virtual const char *getTypeClassName() { return #typeName; }; \
      virtual const char *prepData(const char *data, char *buffer, U32 bufferLen); \
      virtual StringTableEntry getTypePrefix( void ) const { return StringTable->insert( typePrefix ); }\
   }; \
   S32 type = -1; \
   ConsoleType##type gConsoleType##type##Instance(size,&type,#type); \

#define ConsoleSetType( type ) \
   void ConsoleType##type::setData(void *dptr, S32 argc, const char **argv, EnumTable *tbl, BitSet32 flag)

#define ConsoleGetType( type ) \
   const char *ConsoleType##type::getData(void *dptr, EnumTable *tbl, BitSet32 flag)

#define ConsolePrepData( type ) \
   const char *ConsoleType##type::prepData(const char *data, char *buffer, U32 bufferSize)

#define ConsoleTypeFieldPrefix( type, typePrefix ) \
   StringTableEntry ConsoleType##type::getTypePrefix( void ) const { return StringTable->insert( typePrefix ); }

#define DatablockConsoleType( typeName, type, size, className ) \
   class ConsoleType##type : public ConsoleBaseType \
   { \
   public: \
      ConsoleType##type (const S32 aSize, S32 *idPtr, const char *aTypeName) : ConsoleBaseType(aSize, idPtr, aTypeName) { } \
      virtual void setData(void *dptr, S32 argc, const char **argv, EnumTable *tbl, BitSet32 flag); \
      virtual const char *getData(void *dptr, EnumTable *tbl, BitSet32 flag ); \
      virtual const char *getTypeClassName() { return #className; }; \
      virtual const bool isDatablock() { return true; }; \
   }; \
   S32 type = -1; \
   ConsoleType##type gConsoleType##type##Instance(size,&type,#type); \


#endif // _CONSOLE_BASE_TYPE_H_
