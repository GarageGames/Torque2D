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

#ifndef LLIST_H
#define LLIST_H

//***************************************************************************************
//   Linked List
//***************************************************************************************
// This template has an overhead of size LListNode for each entry in the linked list -
//    be aware of this for very large linked lists.
//
// This template has no destructor!  You must explicitly call free() if you want the
//    contents of the list to be destroyed.
//
//
// WARNING - this template has not been thoroughly tested so there may be bugs in it!
//
// -Bramage
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// LListNode template data node
//---------------------------------------------------------------------------------------
template <class T> class LListNode
{
public:
   LListNode<T> *   Next;
   LListNode<T> *   Prev;
   T *            Data;

   LListNode()
   {
      Next = NULL;
      Prev = NULL;
      Data = NULL;
   }

};


//---------------------------------------------------------------------------------------
// LList template
//---------------------------------------------------------------------------------------
template <class T> class LList
{
protected:
   LListNode< T > *first_entry;
   LListNode< T > *last_entry;
   int cnt;

public:

   //---------------------------------------------------------------------------------------
   // Constructor initializes empty list
   //---------------------------------------------------------------------------------------
   LList()
   {
      reset();
   }

   //---------------------------------------------------------------------------------------
   // Reset list to empty state by abandoning contents
   //---------------------------------------------------------------------------------------
   void reset(void)
   {
      first_entry = NULL;
      last_entry  = NULL;
      cnt         = 0;
   }

   //---------------------------------------------------------------------------------------
   // Return entry count
   //---------------------------------------------------------------------------------------
   int size(void) const
   {
      return cnt;
   }

   //---------------------------------------------------------------------------------------
   // Return first list entry (NULL if list empty)
   //---------------------------------------------------------------------------------------
   T *first(void) const
   {
      if( first_entry ){
         return first_entry->Data;
      }
      else
      {
         return NULL;
      }
   }

   //---------------------------------------------------------------------------------------
   // Return last list entry (NULL if list empty)
   //---------------------------------------------------------------------------------------
   T *last(void) const
   {
      if( last_entry )
      {
         return last_entry->Data;
      }
      else
      {
         return NULL;
      }
   }

   //---------------------------------------------------------------------------------------
   // Returns next entry - returns first entry if current == NULL
   //---------------------------------------------------------------------------------------
   T *next( T* current )
   {
      if( current == NULL )
      {
         return first();
      }

      LListNode<T> *next = findNode( current )->Next;
      if( next )
      {
         return next->Data;
      }
      else
      {
         return NULL;
      }
   }

   //---------------------------------------------------------------------------------------
   // Returns prev entry - returns last entry if current == NULL
   //---------------------------------------------------------------------------------------
   T *prev( T *current )
   {
      if( current == NULL )
      {
         return last();
      }

      LListNode<T> *prev = findNode( current )->Prev;
      if( prev )
      {
         return prev->Data;
      }
      else
      {
         return NULL;
      }
   }

   //---------------------------------------------------------------------------------------
   // Link new item into list before specified entry
   // If specified entry==NULL, insert at end of list
   //---------------------------------------------------------------------------------------
   T *link(T *entry, T *next = NULL)
   {
      LListNode<T> *prevNode = NULL;
      LListNode<T> *nextNode = findNode( next );
      LListNode<T> *newNode = new LListNode<T>;

      newNode->Data = entry;

      if( nextNode == NULL)
      {
         prevNode = last_entry;
         last_entry = newNode;
      }
      else
      {
         prevNode = nextNode->Prev;
         nextNode->Prev = newNode;
      }

      if( prevNode == NULL )
      {
         first_entry = newNode;
      }
      else
      {
         prevNode->Next = newNode;
      }

      newNode->Next = nextNode;
      newNode->Prev = prevNode;

      ++cnt;

      return entry;
   }

   //---------------------------------------------------------------------------------------
   // Link new item into list before specified entry
   // If specified entry==NULL, insert at end of list
   //---------------------------------------------------------------------------------------
   T *link(T &entry, T *next = NULL)
   {
      T *newEntry = new T;
      *newEntry = entry;

      return link( newEntry, next );
   }

   //---------------------------------------------------------------------------------------
   // Unlink item from list (without destroying it)
   //---------------------------------------------------------------------------------------
   void unlink(T *entry)
   {
      LListNode<T> *entryNode = findNode( entry );
      if( !entryNode ) return;


      if( entryNode->Prev == NULL )
      {
         first_entry = entryNode->Next;
      }
      else
      {
         entryNode->Prev->Next = entryNode->Next;
      }

      if( entryNode->Next == NULL )
      {
         last_entry = entryNode->Prev;
      }
      else
      {
         entryNode->Next->Prev = entryNode->Prev;
      }

      delete entryNode;

      --cnt;
   }

   //---------------------------------------------------------------------------------------
   // Allocate entry and insert before specified entry
   // If specified entry==NULL, insert at end of list
   //---------------------------------------------------------------------------------------
   T * alloc( T *next = NULL )
   {
      T *entry = new T;

      if( entry == NULL )
      {
         return NULL;
      }

      return link( entry, next );
   }

   //---------------------------------------------------------------------------------------
   // Unlink item from list and destroy it
   //---------------------------------------------------------------------------------------
   void free(T *entry)
   {
      unlink(entry);
      delete entry;
   }

   //---------------------------------------------------------------------------------------
   // Unlink and destroy all list items
   //---------------------------------------------------------------------------------------
   void free(void)
   {
      LListNode<T> *node = NULL;

      while( (node = iterate( node ) )!=NULL )
      {
         LListNode<T> *nodeToKill = node;
         node = node->Prev;
         free( nodeToKill->Data );
      }
   }

   //---------------------------------------------------------------------------------------
   // Find node
   //---------------------------------------------------------------------------------------
   LListNode<T> *findNode( T *entry )
   {
      LListNode<T> *it = NULL;
      while( (it = iterate( it ) )!=NULL )
      {
         if( it->Data == entry )
         {
            return it;
         }
      }
      return NULL;
   }

   //---------------------------------------------------------------------------------------
   // Returns the next node in list
   //---------------------------------------------------------------------------------------
   LListNode<T> *iterate( LListNode<T> *entry = NULL )
   {
      if( entry == NULL ) return first_entry;
      return entry->Next;
   }

};


#endif
