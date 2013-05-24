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

ConsoleMethodGroupBeginWithDocs(ActionMap, SimObject)

/*! Use the bind method to associate a function to a keystroke or other device input.
    The command bound via the bind function must be specified as a flat name with no elipses or semi-colon termination and will be called on make and break events (i.e. key press and release for a mapped key). Args: Warning: When a function is bound to a keystroke or other device input, and no other versions of the binding are provided, the function will be called even if a modifier key like CTRL, ALT, or SHIFT is also pressed.
    @param device Name of the device to bind the command to.
    @param action Name of the action to watch for.
    @param modifier Special modifiers (mouse only), such as dead spot, etc.
    @param command The function to be called on make and break.
    @return No return value.
    @sa bindCmd, getBinding, unbind
*/
ConsoleMethodWithDocs( ActionMap, bind, ConsoleVoid, 5, 10, ( device , action , [ modifier , mod... ] , command ))
{
   object->processBind( argc - 2, argv + 2, NULL );
}

/*! Use the bindObj method to associate a function to a keystroke or other device input.
    The command bound via the bind function must be specified as a flat name with no elipses or semi-colon termination and will be called on make and break events (i.e. key press and release for a mapped key). Args: Warning: When a function is bound to a keystroke or other device input, and no other versions of the binding are provided, the function will be called even if a modifier key like CTRL, ALT, or SHIFT is also pressed.
    @param device Name of the device to bind the command to.
    @param action Name of the action to watch for.
    @param modifier Special modifiers (mouse only), such as dead spot, etc.
    @param command The function to be called on make and break.
    @param object The explicit object (it defaults to NULL when you call bind() )
    @return No return value.
    @sa bindCmd, getBinding, unbind
*/
ConsoleMethodWithDocs( ActionMap, bindObj, ConsoleVoid, 6, 11, (device, action, [modifier spec, mod...], command, object))
{
   SimObject* obj = Sim::findObject(argv[argc - 1]);
   object->processBind( argc - 3, argv + 2, obj );
}

//------------------------------------------------------------------------------
/*! Use the bindCmd method to associate up to two functions to a keystroke or other device input.
    The makeCmd is bound to the make event and the breakCmd is bound to the break event and in both cases, the commands are specified as complete scripts, with all arguments, elipses, and the terminating semi-colon. Either of these commands may be non-specified (NULL strings). For clarification, see 'Bind Sample' example below.
    @param device Name of the device to bind the command to (see 'Device Table' below).
    @param action Name of the action to watch for(see 'Action Table' below).
    @param makeCmd The function to be called on make event.
    @param breakCmd The function to be called on break event.
    @return No return value.
    @sa bind, getBinding, unbind
*/
ConsoleMethodWithDocs( ActionMap, bindCmd, ConsoleVoid, 6, 6, ( device , action , makeCmd , breakCmd ))
{
   object->processBindCmd( argv[2], argv[3], argv[4], argv[5] );
}

//------------------------------------------------------------------------------
/*! Use the unbind method to remove a previosly specified device + action pair from the action map.
    @param device Name of the device to bound to a command (see 'Device Table' below).
    @param action Name of the action to watch for (see 'Action Table' below).
    @return No return value.
    @sa bind, bindCmd
*/
ConsoleMethodWithDocs( ActionMap, unbind, ConsoleVoid, 4, 4, ( device , action ))
{
   object->processUnbind( argv[2], argv[3] );
}
/*! Use the unbind method to remove a previosly specified device + action pair from the action map.
    @param device Name of the device to bound to a command (see 'Device Table' below).
    @param action Name of the action to watch for (see 'Action Table' below).
    @param object Explicit object (it defaults to NULL when you call unbind() ).
    @return No return value.
    @sa bind, bindCmd
*/
ConsoleMethodWithDocs( ActionMap, unbindObj, ConsoleVoid, 5, 5, (device, action, object))
{
   SimObject* obj = Sim::findObject(argv[4]);
   object->processUnbind( argv[2], argv[3], obj );
}


//------------------------------------------------------------------------------
/*! Use the save method to save an entire action map specification to a file. If append is not specified, or specified as false, fileName will be overwritten, otherwise the action map will be appended to the file.
    @param fileName Full path to file in which to store action map definition.
    @param append If true, do not overwrite the file, else start from scratch.
    @return No return value
*/
ConsoleMethodWithDocs( ActionMap, save, ConsoleVoid, 2, 4, ( [ fileName ] [ , append ] ))
{
   const char* fileName = argc > 2 ? argv[2]        : NULL;
   bool append          = argc > 3 ? dAtob(argv[3]) : false;

   char buffer[1024];

   if(fileName)
   {
      if(Con::expandPath(buffer, sizeof(buffer), fileName))       
         fileName = buffer;
   }

   object->dumpActionMap( fileName, append );
}

//------------------------------------------------------------------------------
/*! Use the push method to activate an ActionMap and place it at the top of the non-global ActionMap stack.
    @return No return value.
    @sa pop
*/
ConsoleMethodWithDocs( ActionMap, push, ConsoleVoid, 2, 2, ())
{
   SimSet* pActionMapSet = Sim::getActiveActionMapSet();
   pActionMapSet->pushObject( object );
}

//------------------------------------------------------------------------------
/*! Use the pop method to de-activate an ActionMap and remove it from non-global ActionMap stack.
    @return No return value.
    @sa push
*/
ConsoleMethodWithDocs( ActionMap, pop, ConsoleVoid, 2, 2, ())
{
   SimSet* pActionMapSet = Sim::getActiveActionMapSet();
   pActionMapSet->removeObject( object );
}

//------------------------------------------------------------------------------
/*! Use the getBinding method to get the binding for a specified command.
    @param command The function to seek a binding for.
    @return Returns a string containing the binding as a field (TAB separated string), or a NULL string meaning 'no binding found'.
    @sa bind, bindCmd
*/
ConsoleMethodWithDocs( ActionMap, getBinding, ConsoleString, 3, 3, ( command ))
{
    return( object->getBinding( argv[2] ) );	
}

//------------------------------------------------------------------------------
/*! Use the getCommand method to get the function associated with a specific device + action pair.
    @param device Name of the device to bound to a command (see 'Device Table' below).
    @param action Name of the action to watch for (see 'Action Table' below).
    @return Returns the function name or specification associated with the specified device + action pair, or a NULL-string meaning 'no binding found'.
    @sa bind, bindCmd, getBinding
*/
ConsoleMethodWithDocs( ActionMap, getCommand, ConsoleString, 4, 4, ( device , action ))
{
    return( object->getCommand( argv[2], argv[3] ) );	
}

//------------------------------------------------------------------------------
/*! Use the Purpose method to determine if a specific device + action pair in inverted.
    This only applies to scrolling devices.
    @param device Name of the device to bound to a command (see 'Device Table' below).
    @param action Name of the action to watch for (see 'Action Table' below).
    @return Returns 1 if the mouse (or other scrolling device) is inverted, 0 otherwise.
    @sa bind, bindCmd
*/
ConsoleMethodWithDocs( ActionMap, isInverted, ConsoleBool, 4, 4, ( device , action ))
{
    return( object->isInverted( argv[2], argv[3] ) );
}

//------------------------------------------------------------------------------
/*! Use the getScale method to get the scale associated with a specific device + action pair.
    @param device Name of the device to bound to a command (see 'Device Table' below).
    @param action Name of the action to watch for (see 'Action Table' below).
    @return Returns 1 if no scale is associated with the specified device + action pair, or the mapping was not found.
    @sa bind, bindCmd
*/
ConsoleMethodWithDocs( ActionMap, getScale, ConsoleFloat, 4, 4, ( device , action ))
{
    return( object->getScale( argv[2], argv[3] ) );
}

//------------------------------------------------------------------------------
/*! Use the getDeadZone method to get the dead-zone associated with a specific device + action pair.
    @param device Name of the device to bound to a command (see 'Device Table' below).
    @param action Name of the action to watch for (see 'Action Table' below).
    @return Returns a dead-zone specification, or \0 0\ meaning that there is no dead-zone, or a NULL string meaning the mapping was not found.
    @sa bind, bindCmd
*/
ConsoleMethodWithDocs( ActionMap, getDeadZone, ConsoleString, 4, 4, ( device , action ))
{
    return( object->getDeadZone( argv[2], argv[3] ) );
}

ConsoleMethodGroupEndWithDocs(ActionMap)
