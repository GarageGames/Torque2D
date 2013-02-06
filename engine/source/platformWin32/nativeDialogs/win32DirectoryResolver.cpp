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
#include <windowsx.h>
#include <shlobj.h>
#include <shlwapi.h>

#include "string/unicode.h"
#include "console/consoleTypes.h"
#include "win32DirectoryResolver.h"
//-----------------------------------------------------------------------------
Win32DirectoryResolver::Win32DirectoryResolver() : mPSL(0), mPPF(0)
{
   // COM init
   CoInitialize(NULL);

   // Get a pointer to the IShellLink interface.       
   HRESULT hres = CoCreateInstance( CLSID_ShellLink,
							               NULL,
							               CLSCTX_INPROC_SERVER,
							               IID_IShellLink,
							               (LPVOID*) &mPSL );
   
   // Get a pointer to the IPersistFile interface. 
   if( SUCCEEDED( hres ) ) 	      		      
		mPSL->QueryInterface( IID_IPersistFile, (LPVOID*)&mPPF );	
}
//-----------------------------------------------------------------------------
Win32DirectoryResolver::~Win32DirectoryResolver()
{
   // COM clean up
   if( mPPF )
   {
      mPPF->Release();
      
      mPPF = 0;
   }

   if( mPSL )
   {
      mPSL->Release();

      mPSL = 0;
   }
   
   CoUninitialize();
}
//-----------------------------------------------------------------------------
bool Win32DirectoryResolver::isDirectory( LPSTR strPathName ) const
{
   // Normal directory?
   if( PathIsDirectoryA( strPathName ) )
      return true;

   // Check if it is a shortcut and if so check if it is to a directory
   // Does it have a file extension?      
   const char *pathEnd = strPathName + dStrlen( strPathName );

   if( PathFindExtensionA( strPathName ) != pathEnd )
      return false;
   
   // Ok, so it could be a short cut
   bool result = false;
	
   // Make sure we have the interfaces we need
   if( mPSL && mPPF )
   {      		
      // COM stuff need UTF16/WCHAR
      UTF16 wpath[ MAX_PATH ];  		  
      
      convertUTF8toUTF16( strPathName, wpath, sizeof(wpath) );

      if( !PathAddExtensionW( wpath, L".lnk" ) )
         return false;
      
      // Load the shortcut
      HRESULT hres = mPPF->Load( wpath, STGM_READ ); 
					
		if( SUCCEEDED( hres ) ) 
	   {   
         // Resolve the link
		   hres = mPSL->Resolve( 0, SLR_ANY_MATCH ); 
			
			if( SUCCEEDED( hres ) ) 
			{  
            // Get the path to the link target (reuse buffer)
            WIN32_FIND_DATA wfd;  			   
            
				hres = mPSL->GetPath( wpath, 
										    sizeof(wpath),
										    (WIN32_FIND_DATA *)&wfd, 
										    SLGP_SHORTPATH ); 

            // Is it a directory?
				if( SUCCEEDED( hres ) )
				   result = PathIsDirectory( wpath );
			} 
		}
	}
	   	
   return result; 
}
//-----------------------------------------------------------------------------
/*
bool Win32DirectoryResolver::isDirectory( LPSTR strPathName ) const
{
   // Normal directory?
   if( PathIsDirectoryA( strPathName ) )
      return TRUE;

   // Check if it is a shortcut and if so check if it is to a directory
   // Does it have a file extension?
   //if( PathHas

   // No so it could be a short cut
   bool result = false;

	char shortCutPath[ MAX_PATH ];

   dStrcpy( shortCutPath, strPathName );
   dStrcat( shortCutPath, ".lnk" );
   
   // Get a pointer to the IShellLink interface. 
   IShellLink* psl;
   
   HRESULT hres = CoCreateInstance( CLSID_ShellLink,
							               NULL,
							               CLSCTX_INPROC_SERVER,
							               IID_IShellLink,
							               (LPVOID*) &psl );
   if( SUCCEEDED( hres ) ) 
	{      		
      // Get a pointer to the IPersistFile interface. 
      IPersistFile* ppf;  

		hres = psl->QueryInterface( IID_IPersistFile, (LPVOID*)&ppf );

		if( SUCCEEDED( hres ) ) 
		{ 
		   WORD wsz[MAX_PATH];  
		   
         // Ensure that the string is Unicode. 
			MultiByteToWideChar( CP_ACP,
										0,
										shortCutPath,
										-1,
										wsz, 
										MAX_PATH );   
         
         // Load the shortcut. 
         hres = ppf->Load( wsz, STGM_READ ); 
						
			if( SUCCEEDED( hres ) ) 
		   {   
            // Resolve the link. 
			   hres = psl->Resolve( 0, SLR_ANY_MATCH ); 
				
				if( SUCCEEDED( hres ) ) 
				{  
               WIN32_FIND_DATA wfd;  
				   WCHAR           szGotPath[ MAX_PATH ];

               // Get the path to the link target. 
					hres = psl->GetPath( szGotPath, 
											   MAX_PATH,
											   (WIN32_FIND_DATA *)&wfd, 
											   SLGP_SHORTPATH ); 
																		
					if( SUCCEEDED( hres ) )
					   result = PathIsDirectory( szGotPath );               
				} 
			}
		
         // Release the pointer to the IPersistFile interface. 
		   ppf->Release();         
		} 
	   
      // Release the pointer to the IShellLink interface. 
		psl->Release();
	}
	
   // whether OS is <= NT4 or not... use this helper:
	//if( ShortToLongPathName( LnkPath, sLong ) > LnkPath.GetLength() )
	  // LnkPath = sLong;
	
   return result; 
}*/
//-----------------------------------------------------------------------------