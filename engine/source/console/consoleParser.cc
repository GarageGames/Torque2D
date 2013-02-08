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

#include "platform/platform.h"
#include "console/console.h"
#include "console/consoleParser.h"

namespace Compiler
{

static ConsoleParser *gParserList = NULL;
static ConsoleParser *gDefaultParser = NULL;

void freeConsoleParserList(void)
{
	ConsoleParser *pParser;

	while((pParser = gParserList))
	{
		gParserList = pParser->next;
		delete pParser;
	}

	gDefaultParser = NULL;
}

bool addConsoleParser( const char *ext, fnGetCurrentFile gcf, fnGetCurrentLine gcl, fnParse p, fnRestart r, fnSetScanBuffer ssb, bool def /* = false */)
{
	AssertFatal(ext && gcf && gcl && p && r, "AddConsoleParser called with one or more NULL arguments");

	ConsoleParser *pParser;
	if((pParser = new ConsoleParser))
	{
		pParser->ext = ext;
		pParser->getCurrentFile = gcf;
		pParser->getCurrentLine = gcl;
		pParser->parse = p;
		pParser->restart = r;
		pParser->setScanBuffer = ssb;

		if(def)
			gDefaultParser = pParser;

		pParser->next = gParserList;
		gParserList = pParser;

		return true;
	}
	return false;
}

ConsoleParser * getParserForFile(const char *filename)
{
	char *ptr;

	if(filename == NULL)
		return gDefaultParser;

	if((ptr = dStrrchr((char *)filename, '.')))
	{
		ptr++;

		ConsoleParser *p;
		for(p = gParserList;p;p = p->next)
		{
			if(dStricmp(ptr, p->ext) == 0)
				return p;
		}
	}

	return gDefaultParser;
}

} // end namespace Con
