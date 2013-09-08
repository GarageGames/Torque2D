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

ConsoleMethodGroupBeginWithDocs(SimComponent, SimObject)

/*! Adds additional components to current list.
    @param Up to 62 component names
    @return Returns true on success, false otherwise.
*/
ConsoleMethodWithDocs( SimComponent, addComponents, ConsoleBool, 3, 64, ( compObjName, compObjName2, ... ))
{
   for(S32 i = 2; i < argc; i++)
   {
      SimComponent *obj = dynamic_cast<SimComponent*> (Sim::findObject(argv[i]) );
      if(obj)
         object->addComponent(obj);
      else
         Con::printf("SimComponent::addComponents - Invalid Component Object \"%s\"", argv[i]);
   }
   return true;
}

/*! Removes components by name from current list.
    @param objNamex Up to 62 component names
    @return Returns true on success, false otherwise.
*/
ConsoleMethodWithDocs( SimComponent, removeComponents, ConsoleBool, 3, 64, ( compObjName, compObjName2, ... ))
{
   for(S32 i = 2; i < argc; i++)
   {
      SimComponent *obj = dynamic_cast<SimComponent*> (Sim::findObject(argv[i]) );
      if(obj)
         object->removeComponent(obj);
      else
         Con::printf("SimComponent::removeComponents - Invalid Component Object \"%s\"", argv[i]);
   }
   return true;
}

/*! Get the current component count
    @return The number of components in the list as an integer
*/
ConsoleMethodWithDocs( SimComponent, getComponentCount, ConsoleInt, 2, 2, ())
{
   return object->getComponentCount();
}

/*! Get the component corresponding to the given index.
    @param idx An integer index value corresponding to the desired component.
    @return The id of the component at the given index as an integer
*/
ConsoleMethodWithDocs( SimComponent, getComponent, ConsoleInt, 3, 3, (idx))
{
   S32 idx = dAtoi(argv[2]);
   if(idx < 0 || idx >= (S32)object->getComponentCount())
   {
      Con::errorf("SimComponent::getComponent - Invalid index %d", idx);
      return 0;
   }

   SimComponent *c = object->getComponent(idx);
   return c ? c->getId() : 0;
}

/*! Sets or unsets the enabled flag
    @param enabled Boolean value
    @return No return value
*/
ConsoleMethodWithDocs(SimComponent, setEnabled, ConsoleVoid, 3, 3, (enabled))
{
   object->setEnabled(dAtob(argv[2]));
}

/*! Check whether SimComponent is currently enabled
    @return true if enabled and false if not
*/
ConsoleMethodWithDocs(SimComponent, isEnabled, ConsoleBool, 2, 2, ())
{
   return object->isEnabled();
}

ConsoleMethodGroupEndWithDocs(SimComponent)
