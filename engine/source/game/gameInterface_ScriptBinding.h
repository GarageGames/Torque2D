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

#ifdef TORQUE_ALLOW_JOURNALING

/*! Use the saveJournal function to save a new journal of the current game.
    @param namedFile A full path specifying the file to save this journal to. Usually, journal names end with the extension .jrn.
    @return No return value.
    @sa playJournal
*/
ConsoleFunctionWithDocs( saveJournal, ConsoleVoid, 2, 2, ( namedFile ))
{
   Game->saveJournal(argv[1]);
}

/*! Use the playJournal function to play back a journal from namedFile and to optionally break (into an active debugger) after loading the Journal. This allow us to debug engine bugs by reproducing them consistently repeatedly.
    The journaling system is a vital tool for debugging complex or hard to reproduce engine and script bugs.
    @param namedFile A full path to a valid journal file. Usually, journal names end with the extension .jrn.
    @param doBreak A boolean value. If true, the engine will load the journal and then assert a break (to break into an active debugger). If not true, the engine will play back the journal with no break.
    @return No return value.
    @sa saveJournal
*/
ConsoleFunctionWithDocs( playJournal, ConsoleVoid, 2, 3, ( namedFile , doBreak ))
{
   bool jBreak = (argc > 2)? dAtob(argv[2]): false;
   Game->playJournal(argv[1],jBreak);
}

#endif //TORQUE_ALLOW_JOURNALING
