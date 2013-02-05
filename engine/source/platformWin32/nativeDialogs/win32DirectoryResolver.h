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
#ifndef _WIN32DIRECTORYRESOLVER_H_
#define _WIN32DIRECTORYRESOLVER_H_

/// Utility class to resolve a path name and check if it is a directory.
/// If the path points to a short cut it will resolve the real path to
/// the short cut target and check if that is a directory. This allows 
/// listing of short cuts to directories in the file dialogs.
class Win32DirectoryResolver
{
   private:
      IShellLink*    mPSL; /// We cache the IShellLink interface pointer
      IPersistFile*  mPPF; /// We cache the IPersistFile interface pointer 
   
   public:
      /// Constructor
      Win32DirectoryResolver();
      
      /// Destructor
      virtual ~Win32DirectoryResolver();

      /// Check if a file path points to a directory or if it is a shortcut
      /// resolve the short cut path and see if that points to a directory.
      bool isDirectory( LPSTR strPathName ) const;
};

#endif