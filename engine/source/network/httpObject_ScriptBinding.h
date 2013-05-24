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

ConsoleMethodGroupBeginWithDocs(HTTPObject, TCPObject)

/*! 
*/
ConsoleMethodWithDocs( HTTPObject, get, ConsoleVoid, 4, 5, (TransportAddress addr, string requirstURI, string query=NULL))
{
   object->get(argv[2], argv[3], argc == 4 ? NULL : argv[4]);
}

/*! 
*/
ConsoleMethodWithDocs( HTTPObject, post, ConsoleVoid, 6, 6, (TransportAddress addr, string requestURI, string query, string post))
{
   object->post(argv[2], argv[3], argv[4], argv[5]);
}

ConsoleMethodGroupEndWithDocs(HTTPObject)
